/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <memory>
#include "wbuffer.h"
#include "wiodeviceerrno.h"
#include "wiodevice_fwd.h"
#include "abstractioprotocol.h"
#include "wmacros.h"

class WSRPolicy;
class WAbstractPacketModule;
/*
 * IO设备基类
 */
class WIODevice
{
	public:
		typedef WBuffer IOBuffer;
	private:
		/*
		 * 用于连接应用层协议与操作系统实现的底层协议(隐含在IO设备read/write函数中)
		 */
		struct IOBridge:public WAbstractIOProtocol
		{
			inline IOBridge(WIODevice& io):io_dev_(io){}
			virtual int read(WBuffer* data,WIODevice* io)override;
			virtual int write(WBuffer* data,WIODevice* io)override;
			WIODevice& io_dev_;
		};
	public:
		WIODevice(unsigned type=0);
		/*
		 * 打开设备，使设备处理可使用状态
		 */
		virtual bool open();
		/*
		 * 关闭设备
		 */
		virtual bool close();
		/*
		 * 判断设备是否可用于收发数据
		 */
		virtual bool isValid()const=0;
		/*
		 * 用于判断设备是否打开
		 */
		virtual bool isOpen()const;
		/*
		 * 发送一个数据，数据会经协议处理后再发送
		 */
		int send(const WBuffer& data);
		int send(WBuffer* data);
		inline int fd()const { return fd_; }
		/*
		 * 接收一组数据，数据会经过协议处理后再返回
		 */
		int recv(WBuffer* data);
		/*
		 * 根据定义可能比raw_read多出数据监测的功能
		 */
		int read(WBuffer* data);
		/*
		 * 根据定义可能比raw_write多出数据监测的功能
		 */
		int write(WBuffer* data);
		/*
		 * time_out单位毫秒
		 */
		bool waitForReadyRead(int time_out);
		/*
		 * 暂停长读操作，如果成功，返回true
		 */
		virtual int pauseRead();
		virtual int setBlock(bool isblock);
		/*
		 * IO类型
		 */
		inline unsigned id()const { return id_;}
#ifdef ENABLE_IO_DEVICE_MONITOR
		inline void setIOMonitorFunc(IOMonitorFunc func,void* private_data=nullptr) {
			io_monitor_func_    =   func;
			iom_private_data_   =   private_data;
		}
		inline void setIOMonitorFunc(WIOMonitorData data) {
			io_monitor_func_    =   data.func;
			iom_private_data_   =   data.private_data;
		}
#endif
		/*
		 * 设置flag指定的标志
		 */
		int setFDFlag(int flag);
		int resetFDFlag(int flag);
		int getFDFlag()const;
		void clearCache();
		virtual ~WIODevice();
		virtual int getOption(WIODeviceOption option,void* data)const;
		virtual int setOption(WIODeviceOption option,void* data);
		static std::string directionToString(WIODeviceDirection direction);
		inline unsigned flag()const { return flag_; }
		inline void setFlag(unsigned f) { flag_ = f; }
	public:
		inline const WIOProtocols& protocols()const { return protocols_; }
		template<class Protocols>
			void setProtocols(Protocols&& protocols) {
				protocols_ = std::forward<Protocols>(protocols);
				WAbstractIOProtocol::setProtocols(&protocols_,&io_bridge_);
			}
		/*
		 * 删除并释放所有的应用层协议
		 */
		void clearProtocols();
		/*
		 * !!!protocol所有权由WIODevice保留
		 */
		void __pushProtocolBack(WAbstractIOProtocol* protocol);
		/*
		 * !!!protocol所有权由WIODevice保留
		 */
		void __pushProtocolFront(WAbstractIOProtocol* protocol);
		/*
		 * 设置io的应用层协议为P(args...)
		 */
		template<typename P,typename ...Args>
			void setProtocol(Args&&... args) {
				clearProtocols();
				__pushProtocolBack(new P{std::forward<Args>(args)...});
			}
		/*
		 * 在当前io的应用层协议后添加协议为P(args...)
		 */
		template<typename P,typename ...Args>
			void pushProtocolBack(Args&&... args) {
				__pushProtocolBack(new P{std::forward<Args>(args)...});
			}
		/*
		 * 在当前io的应用层协议前添加协议为P(args...)
		 */
		template<typename P,typename ...Args>
			void pushProtocolFront(Args&&...args) {
				__pushProtocolFront(new P{std::forward<Args>(args)...});
			}
	public:
		virtual std::string strInfo()const;
		void dumpInfo()const;
		virtual WAbstractIOProtocol* clone()const;
	protected:
		int          fd_;
		unsigned     flag_;
		IOTimeout    timeout_;
		/*
		 * 应用层协议
		 * 发送时，由前面的协议发起发送，前面的协议先自己处理，然后再使用后面的协议处理
		 * 接收时，由前面的协议发启接收，前面的协议先使用后面的协议处理，然后自己再处理
		 * 为了表示简单，一个协议仅知道自己后面的协议是什么(WAbstractIOProtocol::next_)
		 * 为了处理灵活，协议如何调用后面的协议完成由协议自己定义
		 */
		WIOProtocols protocols_;
		IOBridge     io_bridge_;
#ifdef ENABLE_IO_DEVICE_MONITOR
		/*
		 * 只要读或写了数据，不管有没有解包成功，都会被监视
		 */
		IOMonitorFunc  io_monitor_func_;
		void          *iom_private_data_;
#endif
	protected:
		/*
		 * 直接向设备写入数据
		 * 成功返回写入的字节数，失败返回0,出错返回-1
		 * 在出错的情况下，IO设备可能自动关闭IO
		 */
		virtual int raw_write(const WBuffer& data);
		/*
		 * 直接从设备读取数据
		 * 成功返回读取的字节数，失败返回0,出错返回-1
		 * 在出错的情况下，IO设备可能自动关闭IO
		 */
		virtual int raw_read(WBuffer* data);
	private:
		/*
		 * 设备标识
		 */
		unsigned id_;
	private:
		DISABLE_COPY_AND_ASSIGN(WIODevice);
};
