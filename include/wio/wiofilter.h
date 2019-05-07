/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
 
_Pragma("once")

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_utree.hpp>
#include <string>
#include <stack>
#include <mutex>
#include "wioanalysis_fwd.h"

class WIOFilter
{
	struct symbols0:public boost::spirit::qi::symbols<char,unsigned>
	{
		inline symbols0(){
			add("id"  ,  0)
			("type"   ,  1)
			("read"   ,  2)
			("write"  ,  3)
			("len"    ,  4)
				;
		}
	};
	private:
		using rule = boost::spirit::qi::rule<std::string::const_iterator,
			  boost::spirit::ascii::space_type,
			  unsigned ()>;
	public:
		WIOFilter();
		bool setRule(const std::string& rule);
		/*
		 * 判断data是否满足当前规则
		 */
		unsigned eval(std::weak_ptr<WIOADataItem> data);
		inline std::string get_rule()const { return rule_; }
	private:
		void initExpression();
		void initTerm();
		void initFactor();
		unsigned eval(const std::lock_guard<std::mutex>& guard)noexcept(false);
		/*
		 * 解析symbols
		 */
		unsigned symbols(unsigned v);
	private:
		rule                                 expression_;
		rule                                 term_;
		rule                                 factor_;
		std::string                          rule_;
		std::mutex                           mtx_;
		symbols0                             symbols0_;
		std::weak_ptr<WIOADataItem> data_;
};
