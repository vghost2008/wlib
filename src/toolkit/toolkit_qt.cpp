#include <toolkit_qt.h>
#include <toolkit.h>
#include <QtCore>
#include <QtGlobal>
#include <functional>
#include "wlogmacros.h"

namespace WSpace
{
	typedef QString (*StringTransFunc)(const unsigned char* data,int size,int unit_nr,int column);
	QString getAvailableNumberFileName(const QString& dir_path,const QString& prefix,const QString& suffix,unsigned* begin_index ) 
	{
		const unsigned kMaxImageIndex = 9999;
		unsigned       index          = 1;
		QString        file_path;

		if(!QDir(dir_path).exists()
				&& !QDir().mkpath(dir_path)) {
			ERROR_LOG("Get save dir path faild!");
			return QString();
		}
		if(nullptr != begin_index && *begin_index < kMaxImageIndex)
			index = *begin_index;

		do {
			file_path = QDir(dir_path).absoluteFilePath(prefix+QString("%1").arg(index,4,10,QChar('0'))+suffix);
			++index;
		} while(QFile::exists(file_path)&&index<=kMaxImageIndex);
		if(index>kMaxImageIndex) {
			ERROR_LOG("Get file name faild!");
			return QString();
		}
		if(nullptr != begin_index)*begin_index = index;
		return file_path;
	}
	QString sqlQueryStr(const QString& str)
	{
		auto res = str;
		res.replace("'","''");
		res.replace("\"","\"\"");
		return addSingleQuotes(res);
	}
	QString addSingleQuotes(const QString& str)
	{
		return QString("\'")+str+"\'";
	}
	QString toHexString(const QByteArray& ba,int unit_nr,int column)
	{
		return toHexString((unsigned char*)ba.data(),ba.size(),unit_nr,column).c_str();
	}
	QString toHexQString(const unsigned char* data,int size,int unit_nr,int column)
	{
		return toHexString(data,size,unit_nr,column).c_str();
	}
	QString toBinString(const unsigned char* data,int size,int unit_nr,int column)
	{
		int       i;
		int       j;
		QString   ret_val;
		const int elements_every_row = unit_nr*column;

		for(i=0; i<size;) {
			for(j=0; j<elements_every_row&&i<size; ++j,++i) {
				if(j%unit_nr== 0) ret_val += "    ";
				ret_val += QString("%1").arg((uint)data[i],8,2,QChar('0'))+" ";
			}
			ret_val += "\n";
		}
		return ret_val;
	}
	QString toBinString(const QByteArray& ba,int unit_nr,int column)
	{
		return toBinString((unsigned char*)ba.data(),ba.size(),unit_nr,column);
	}
	int getTransStringColumnNr(int width,const QFontMetrics& fm,int unit_nr,StringTransFunc func)
	{
		int           i;
		unsigned char buffer[2049];
		int           column       = 1;
		const int     max_column   = (ARRAY_SIZE(buffer)-1)/unit_nr;
		QString       out_str;

		width -= fm.width("MM");

		for(i=0; i<ARRAY_SIZE(buffer); ++i) 
			buffer[i] = 'M';
		buffer[ARRAY_SIZE(buffer)-1] = '\0';
		out_str = func(buffer,unit_nr,unit_nr,1).simplified()+"MMMM";
		const int unit_width = fm.width(out_str);	
		if(unit_width==0) return 3;
		column = std::min<float>(float(width)/unit_width,max_column);
		if(column <= 1) return 1;

		for(; column>=1; --column) {
			out_str = func(buffer,column*unit_nr,unit_nr,column);
			if(fm.width(out_str)<width) break;
		}
		return column;
	}
	int getToHexStringColumnNr(int width,const QFontMetrics& fm,int unit_nr)
	{
		return getTransStringColumnNr(width,fm,unit_nr,toHexQString);
	}
	int getToBinStringColumnNr(int width,const QFontMetrics& fm,int unit_nr)
	{
		return getTransStringColumnNr(width,fm,unit_nr,toBinString);
	}
	QString dataSizeToStringBits(unsigned s) {
		const QString name_array[] = {"G","M","K","Bits"};
		return dataSizeToString(s,name_array);
	}
	QString dataSizeToStringByte(unsigned s) {
		const QString name_array[] = {"G","M","K","Byte"};
		return dataSizeToString(s,name_array);
	}
	QString dataSizeToString(unsigned s,const QString name_array[]) {
		char buffer[64];
		if(s>1024*1024*1024) {
			snprintf(buffer,sizeof(buffer),"%.4f%s",float(s)/float(1<<30),name_array[0].toUtf8().data());
		} else if(s>10*1024*1024) {
			snprintf(buffer,sizeof(buffer),"%.4f%s",float(s)/float(1<<20),name_array[1].toUtf8().data());
		} else if(s>10*1024) {
			snprintf(buffer,sizeof(buffer),"%.4f%s",float(s)/float(1<<10),name_array[2].toUtf8().data());
		} else {
			snprintf(buffer,sizeof(buffer),"%u%s",s,name_array[3].toUtf8().data());
		}
		return buffer;
	}
	QStringList filePathListInDir(const QString& dir_path,const QStringList& filter) 
	{
		QStringList   file_path_list;
		QDir          dir(dir_path);
		QFileInfoList info_list      = dir.entryInfoList(filter,QDir::Dirs|QDir::Files|QDir::AllDirs|QDir::NoDot|QDir::NoDotDot);

		for(int i=0; i<info_list.size(); ++i) {
			if(info_list.at(i).isDir()) {
				file_path_list<<filePathListInDir(info_list.at(i).absoluteFilePath(),filter);
			} else {
				file_path_list<<info_list.at(i).absoluteFilePath();
			}
		}
		return file_path_list;
	}
	QStringList fileListInDir(const QString& dir_path,const QStringList& filter)
	{
		QStringList file_list;
		QStringList file_path_list = filePathListInDir(dir_path,filter);

		for(int i=0; i<file_path_list.size(); ++i) {
			file_list<<QFileInfo(file_path_list.at(i)).baseName();
		}
		return file_list;
	}
	QStringList fileNameListInDir(const QString& dir_path,const QStringList& filter)
	{
		QStringList file_list;
		QStringList file_path_list = filePathListInDir(dir_path,filter);

		for(int i=0; i<file_path_list.size(); ++i) {
			QString file_name;
			auto    file_path = file_path_list.at(i);
			auto    index     = file_path.indexOf(dir_path);
			if(-1 == index) {
				file_list<<file_path;
			} else {
				if(file_path.size()<=dir_path.size())continue;
				file_name = file_path.right(file_path.size()-dir_path.size());
				if(file_name.left(1) == "/"
						|| file_name.left(1) == "\\")
					file_name = file_name.right(file_name.size()-1);
				file_list<<file_name;
			}
		}
		return file_list;
	}
	bool getPluginPathByName(const QString& name,const QString& dir_path,QString* path)
	{
#ifdef Q_OS_LINUX
		if(name.size()<3||!name.startsWith("lib")) {
			if(getPluginPathByName(QString("lib")+name,dir_path,path)) return true;
		}
#endif
		bool ret_val = false;
		QDir dir(dir_path);
		if(!dir.exists()) {
			QALL_LOG(LL_ERROR,QString("插件目录不存在, dir=")+dir_path);
			return false;
		}
		QFileInfoList file_info_list = dir.entryInfoList();
		for(int i=0; i<file_info_list.size(); ++i) {
			if(!file_info_list.at(i).isFile())continue;
			if(QString::compare(file_info_list.at(i).baseName(), name,Qt::CaseInsensitive)!=0)continue;
			if(!QLibrary::isLibrary(file_info_list.at(i).absoluteFilePath())) continue;
			*path = file_info_list.at(i).absoluteFilePath();
			ret_val = true;
			break;
		}
		return ret_val;
	}
	QByteArray transDecoding(QByteArray& ba,const QByteArray& code) 
	{
		QTextCodec* codec = QTextCodec::codecForName(code);
		QTextStream stream(&ba);
		stream.setCodec(codec);
		return stream.readAll().toUtf8();
	}
	QByteArray transDecoding(const char* str,const QByteArray& code) 
	{
		QByteArray ba(str,strlen(str));
		return transDecoding(ba,code);
	}
	bool checkPath(QString* path) {
		if(path->startsWith("~")) {
			*path = path->right(path->size()-1);
			if(path->startsWith(QChar(QDir::separator()))) {
				*path = path->right(path->size()-1);
			}
			*path = QDir::home().absoluteFilePath(*path);
		}
		return true;
	}
	bool removeFileIf(const QString& dir_path,const std::function<bool(const QString&)>& filter)
	{
		QDir          dir(dir_path);
		QFileInfoList info_list      = dir.entryInfoList(QDir::Dirs|QDir::Files|QDir::AllDirs|QDir::NoDot|QDir::NoDotDot);

		for(int i=0; i<info_list.size(); ++i) {
			auto file_path = info_list.at(i).absoluteFilePath();
			if(info_list.at(i).isDir()) {
				removeFileIf(file_path,filter);
			} else {
				if(filter(file_path))
					QFile::remove(file_path);
			}
		}
		return true;
	}
	bool rmdir(const QString& dir_path) {
		QDir          dir(dir_path);
		QFileInfoList info_list      = dir.entryInfoList(QDir::Dirs|QDir::Files|QDir::AllDirs|QDir::NoDot|QDir::NoDotDot);

		for(int i=0; i<info_list.size(); ++i) {
			auto file_path = info_list.at(i).absoluteFilePath();
			if(info_list.at(i).isDir()) {
				rmdir(file_path);
			} else {
				QFile::remove(file_path);
			}
		}
		QDir().rmdir(dir_path);
		return true;
	}
	bool copy_file(const QString& src, const QString& dst) {
		if(false == QFile::copy(src,dst)) return false;
		QFile::setPermissions(dst,QFileDevice::ReadUser|QFileDevice::WriteUser|QFileDevice::ReadGroup|QFileDevice::WriteGroup);
		return true;
	}
	bool copy_dir(const QString& src,const QString& dst) {
		auto info0 = QFileInfo(src);
		auto info1 = QFileInfo(dst);

		if(!info0.exists() || !info0.isDir()) return false;
		if(info1.exists() && !info1.isDir()) return false;
		if(!info1.exists()) {
			qDebug()<<"mkpath:"<<dst;
			QDir().mkpath(dst);
		}

		QDir dir0(src);
		QDir dir1(dst);
		auto info_list =  dir0.entryInfoList(QDir::Dirs|QDir::Files|QDir::AllDirs|QDir::NoDot|QDir::NoDotDot);

		for(auto& info:info_list) {
			if(info.isFile()) {
				auto dst_file = dir1.absoluteFilePath(info.fileName());
				if(false == copy_file(info.absoluteFilePath(),dst_file)) return false;
			} else if(info.isDir()) {
				auto dst_dir = dir1.absoluteFilePath(info.fileName());
				if(false == copy_dir(info.absoluteFilePath(),dst_dir)) return false;
			}
		}
		return true;
	}
	bool copy(const QString& src,const QString& dst) {
		auto info = QFileInfo(src);
		if(info.isFile()) {
			return copy_file(src,dst);
		}
		if(info.isDir())  {
			return copy_dir(src,dst);
		}
		return false;
	}
	void assign(char* data,const QString& str,size_t size)
	{
		if(size>0)
			snprintf(data,size,"%s",str.toUtf8().data());
		else
			sprintf(data,"%s",str.toUtf8().data());
	}
}
