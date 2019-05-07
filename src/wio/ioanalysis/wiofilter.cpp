#include <wiofilter.h>
#include <wlogmacros.h>
#include <stdint.h>
#include <wiodevice_fwd.h>
#include <stdexcept>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_function.hpp>

using namespace std;
namespace qi      =  boost::spirit::qi;
namespace ascii   =  boost::spirit::ascii;
namespace spirit  =  boost::spirit;
using qi::uint_;
using qi::char_;
using qi::lit;
using qi::as;
using qi::_val;
using qi::_1;

WIOFilter::WIOFilter()
{
	data_.reset();
	initExpression();
	initTerm();
	initFactor();
}
bool WIOFilter::setRule(const std::string& rule)
{
	lock_guard<mutex> guard{mtx_};

	if(rule.empty()) {
		rule_.clear();
		/*
		 * 规则为空时必须返回false，这样过滤器才不需要做不必要的过滤操作
		 */
		return false;
	}

	data_.reset();
	rule_ = rule;

	try {
		eval(guard);
	} catch(...) {
		rule_.clear();
		return false;
	}
	return true;
}
unsigned WIOFilter::eval(weak_ptr<WIOADataItem> data)
{
	lock_guard<mutex> guard{mtx_};

	data_ = data;
	try {
		return eval(guard);
	} catch(...) {
		return -1;
	}
}
unsigned WIOFilter::eval(const lock_guard<mutex>& )noexcept(false)
{
	unsigned res = 0;

	if(boost::spirit::qi::phrase_parse(rule_.cbegin(),rule_.cend(),expression_,ascii::space,res)) {
		return res;
	}
	throw runtime_error("eval faild.");
}
void WIOFilter::initExpression()
{
	expression_ =
		term_[_val=_1]
		>> *(lit("||") >> term_[_val = (_val||_1)]
			)
		;
}
void WIOFilter::initTerm()
{
	term_ =
		factor_[_val = _1]
		>> *(    
				  (lit("==") >> factor_[_val = (_val==_1)])
				| (lit("!=") >> factor_[_val = (_val!=_1)])
				| (lit("<=") >> factor_[_val = (_val<=_1)])
				| (lit(">=") >> factor_[_val = (_val>=_1)])
				| (char_("<") >> factor_[_val = (_val<_1)])
				| (char_(">") >> factor_[_val = (_val>_1)])
				| (lit("&&") >> factor_[_val = (_val&&_1)])
				| (lit("&") >> factor_[_val = (_val&_1)])
				| ((lit("|")-lit("||")) >> factor_[_val = (_val|_1)])
			)
			;
}
void WIOFilter::initFactor()
{
	factor_ %=
		(lit("0x")>>(boost::spirit::hex))
		| uint_
		| (lit("data[")>>uint_[([this](unsigned & v) {
					auto shared_data = data_.lock();
					if(shared_data == nullptr|| shared_data->data.size()<= v) {
					v = 0;
					}  else {
					v = (uint8_t)(shared_data->data.data()[v]);
					}
					})]>>char_(']'))
	| (lit("word[")>>uint_[([this](unsigned& v) {
				auto shared_data = data_.lock();
				if(shared_data == nullptr|| shared_data->data.size()<=v*2) {
				v = 0;
				}  else {
				v = *(shared_data->data.cast<const uint16_t*>(v*2));
				}
				})]>>char_(']'))
	| (lit("dword[")>>uint_[([this](unsigned& v) {
				auto shared_data = data_.lock();
				if(shared_data == nullptr|| shared_data->data.size()<=v*4) {
				v = 0;
				}  else {
				v = *(shared_data->data.cast<const uint32_t*>(v*4));
				}
				})]>>char_(']'))
	| (lit("contain(")>>factor_[([this](unsigned& v){
		auto shared_data = data_.lock();

		if(shared_data == nullptr) {
		 	v = 0;
			return;
		}

		v = (shared_data->data.search(v)!=-1);
	})]>>char_(')'))
	| (symbols0_[([this](unsigned& v) {
				v = symbols(v);
				})])
	| '(' >> expression_ >> ')'
		| (char_('-') >> factor_[([](unsigned& v){ 
					v = -v;
					})])
	| (char_('+') >> factor_)
		;
}
unsigned WIOFilter::symbols(unsigned v)
{
	auto res  =  v;

	switch(v) {
		case 0:
			{
				auto shared_data = data_.lock();
				if(shared_data == nullptr) {
					res =-1;
				} else {
					res = shared_data->id;
				}
			}
			break;
		case 1:
			{
				auto shared_data = data_.lock();
				if(shared_data == nullptr) {
					res = 0;
				} else {
					res =  shared_data->direction;
				}
			}
			break;
		case 2:
			res = WIODD_READ;
			break;
		case 3:
			res = WIODD_WRITE;
			break;
		case 4:
			{
				auto shared_data = data_.lock();
				if(shared_data == nullptr) {
					res = 0;
				} else {
					res = shared_data->data.size();
				}
			}
			break;
	}
	return res;
}
