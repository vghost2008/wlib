/********************************************************************************
 *   License     : GPL
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
 
_Pragma("once")
#include <iterator>
#include <boost/iterator/iterator_facade.hpp>

template<class T,int Skip>
class skip_iterator
{
	public:
		skip_iterator(T begin,T end):begin_(begin),end_(end){}
		template<class VT>
			class skip_iter:public boost::iterator_facade<skip_iter<VT>,VT,boost::forward_traversal_tag>
		{
			public:
				skip_iter(T v,T begin):v_(v),begin_(begin) {
					if((v_ == begin_) && (Skip==0))
						++v_;
				}
				bool equal(const skip_iter<VT>& v)const { return v_ == v.v_; }
				void increment() { 
					++v_;
					if(std::distance(begin_,v_) == Skip) 
						++v_;
				}
				VT& dereference()const { return *v_; }
			private:
				T v_;
				T begin_;
		};
		typedef skip_iter<typename std::iterator_traits<T>::value_type> iterator;
		typedef skip_iter<const typename std::iterator_traits<T>::value_type> const_iterator;

		iterator begin() { return iterator(begin_,begin_);}
		iterator end() { return iterator(end_,begin_);}
		const_iterator begin()const { return const_iterator(begin_,begin_);}
		const_iterator end()const { return const_iterator(end_,begin_);}
		const_iterator cbegin()const { return const_iterator(begin_,begin_);}
		const_iterator cend()const { return const_iterator(end_,begin_);}
	private:
		T begin_;
		T end_;
};
template<int Skip,class T>
inline skip_iterator<T,Skip> make_skip_iterator(T begin,T end) 
{
	return skip_iterator<T,Skip>(begin,end);
}
template<int Skip,class CT>
inline auto make_skip_iterator(CT& ct)->skip_iterator<decltype(std::begin(ct)),Skip>
{
	using T = decltype(std::begin(ct));
	return skip_iterator<T,Skip>(std::begin(ct),std::end(ct));
}
