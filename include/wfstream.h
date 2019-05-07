_Pragma("once")
#include <fstream>
#include <stdint.h>

namespace WSpace {
	class wfstream:public std::fstream
	{
		public:
			using std::fstream::fstream;
			~wfstream();
			void open(const char* filename,std::ios_base::openmode mode=std::ios_base::in|std::ios_base::out);
			void open(const std::string& filename,std::ios_base::openmode mode=std::ios_base::in|std::ios_base::out);
			inline void setHeader(const std::string& header=std::string()) {
				header_ = header;
			}
			inline const std::string& filename()const { return filename_; }
			void setDeletaIfEmpty(bool v) {
				delete_if_empty_ = v;
			}
			uint64_t size()const;
			static uint64_t size(const std::string& filename);
			bool empty()const;
		private:
			std::string filename_;
			std::string header_;
			bool        delete_if_empty_ = true;
	};
}
