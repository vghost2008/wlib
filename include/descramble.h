_Pragma("once")
#include <stdint.h>
#include <vector>
template<typename T>
class Descramble
{
	public:
		Descramble(T poly,T initial_phase=(~0u))
		:init_phase_(initial_phase)
		,phase_(initial_phase)
		{
			for(auto i=0; i<(sizeof(T)<<3); ++i) {
				if(poly&(1u<<i)) poly_.push_back(i);
			}
		}
		/*
		 * 仅用于最高一位
		 */
		uint8_t apply_one_bit(uint8_t v) {
			auto d      =  (uint8_t(phase_)^(v>>7))&0x01;
			auto new_v = 0;
			for(auto ind:poly_) {
				new_v ^= (phase_>>ind)&0x01;
			}
			phase_>>= 1;
			phase_ |= (new_v<<(bits_of_t_-1));
			return d;
		}
		uint8_t apply(uint8_t v){
			uint8_t res = 0;
			for(int i=7; i>=0; --i) {
				res |= (apply_one_bit(v)<<i);
				v <<=1;
			}
			return res;
		}
		void apply(void* _data,size_t size) {
			auto data = reinterpret_cast<uint8_t*>(_data);
			for(auto i=0; i<size; ++i) {
				auto v = apply((uint8_t)data[i]);
				data[i] = v;
			}
		}
		void init() {
			phase_ = init_phase_;
		}
	private:
		T                       init_phase_ = ~(0u);
		T                       phase_      = ~(0u);
		std::vector<uint8_t>    poly_;
		static constexpr size_t bits_of_t_  = sizeof(T)*8;
};
