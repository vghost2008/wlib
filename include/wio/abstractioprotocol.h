/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
 
_Pragma("once")
#include <vector>
#include <boost/ptr_container/ptr_vector.hpp>
class WIODevice;
class WBuffer;

class WAbstractIOProtocol;
typedef boost::ptr_vector<WAbstractIOProtocol> WIOProtocols;
class WIODevice;
class WAbstractIOProtocol
{
	public:
		WAbstractIOProtocol();
		virtual ~WAbstractIOProtocol();

		/*
		 * 成功返回写入/读取的字节数
		 * 失败返回0,出错返回-1
		 */
		virtual int write(WBuffer* data,WIODevice* io);
		virtual int read(WBuffer* data,WIODevice* io);
		int next_write(WBuffer* data,WIODevice* io);
		int next_read(WBuffer* data,WIODevice* io);
		virtual bool start();
		virtual bool stop();
		virtual void doClearCache();
		void clearCache();
		virtual WAbstractIOProtocol* clone()const;
		/*
		 * 使protocols[i]的next为protocols[i+1],protocols中最后一个的next为io
		 */
		static void setProtocols(WIOProtocols* protocols,WAbstractIOProtocol* io);
		inline WAbstractIOProtocol* next(){ return next_; }
		inline const WAbstractIOProtocol* next()const { return next_; }
	public:
		inline void clearError() { errno_ = 0; }
		inline unsigned getErrno()const { return errno_; }
		inline void setErrno(unsigned e) { errno_ = e; }
	private:
		WAbstractIOProtocol *next_  = nullptr;
		unsigned             errno_;
};
inline WAbstractIOProtocol* new_clone(const WAbstractIOProtocol& v)
{
	return v.clone();
}
