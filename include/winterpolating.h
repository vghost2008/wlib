/********************************************************************************
 *   License     : GPL
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
 
_Pragma("once")
#include <boost/iterator/iterator_facade.hpp>
#include <iterator>
#include <assert.h>
template<typename T>
class WInterpolating
{
	public:
		using value_type = typename T::value_type;
		using t_iterator = typename T::iterator;
		using t_const_iterator = typename T::const_iterator;
	public:
		template<typename t_iterator,typename data_type>
			class comm_iter:public boost::iterator_facade<comm_iter<t_iterator,data_type>,data_type,boost::forward_traversal_tag>
		{
			public:
				using value_type = std::remove_const_t<data_type>;
			public:
				comm_iter(t_iterator it,t_iterator end,unsigned inter_nr):it_(it),it_end_(end),inter_nr_(inter_nr){
					assert(inter_nr_>0);
					set_current_value();
				}
				void increment() {
					if(it_end_ == it_) return;
					if((inter_pos_<inter_nr_-1) && (it_end_ != next(it_))) {
						++inter_pos_;
					} else {
						++it_;
						inter_pos_ = 0;
					}
					set_current_value();
				}
				bool equal(const comm_iter& other)const {
					return (it_==other.it_)&&(inter_pos_==other.inter_pos_);
				}
				const value_type& dereference()const {
					return current_value_;
				}
				data_type& dereference(){
					return current_value_;
				}
			private:
				void set_current_value()
				{
					if(it_end_ == it_) return;
					if(0 == inter_pos_) {
						current_value_ = *it_;
						return;
					}
					const auto& value0 = *it_;
					const auto& value1 = *next(it_);

					current_value_ = value0+(value1-value0)*double(inter_pos_)/inter_nr_;
				}
			private:
				t_iterator       it_;
				const t_iterator it_end_;
				const unsigned   inter_nr_      = 0;
				unsigned         inter_pos_     = 0;
				std::remove_const_t<data_type>       current_value_;
		};
		using iterator=comm_iter<typename T::iterator,typename std::iterator_traits<typename T::iterator>::value_type>;
		using const_iterator=comm_iter<typename T::const_iterator,const typename std::iterator_traits<typename T::iterator>::value_type>;
	public:
		/*
		 * inter_nr:把相邻的数分为inter_nr份，数据间隔为(data[i]-data[i-1])/inter_nr
		 */
		template<typename DT>
			WInterpolating(DT&& data,unsigned inter_nr):data_(std::forward<DT>(data)),inter_nr_(inter_nr){
				assert(inter_nr_>0);
			}
		inline unsigned inter_nr()const { return inter_nr_; }
		inline const_iterator begin()const{ return const_iterator(std::begin(data_),std::end(data_),inter_nr_);}
		inline const_iterator end()const{ return const_iterator(std::end(data_),std::end(data_),inter_nr_);}
		inline const_iterator cbegin()const{ return const_iterator(std::begin(data_),std::end(data_),inter_nr_);}
		inline const_iterator cend()const{ return const_iterator(std::end(data_),std::end(data_),inter_nr_);}
	private:
		T              data_;
		const unsigned inter_nr_ = 0;
};
template<typename T>
WInterpolating<std::decay_t<T>> make_interpolating(T&& data,unsigned inter_nr) {
	return WInterpolating<std::decay_t<T>>(std::forward<T>(data),inter_nr);
}
