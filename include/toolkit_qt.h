/********************************************************************************
 *   License     : GPL
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
 
_Pragma("once")
#include <QString>
#include <QStringList>
#include <QFontMetrics>
#include <QFileInfo>
#include <QDateTime>
#include <functional>

namespace WSpace
{
	/*
	 * 在目录中获取一个数字递增的文件名，如IMG_0001.jpeg, (prefix="IMG_",suffix=".jpeg")
	 */
	QString getAvailableNumberFileName(const QString& dir_path,const QString& prefix,const QString& suffix,unsigned* begin_index=nullptr);
	/*
	 * 在字符串两边加上单引号
	 */
	QString addSingleQuotes(const QString& str);
	/*
	 * 将str转换为一个合法的sql语句
	 */
	QString sqlQueryStr(const QString& str);
	/*
	 * 显示时一行分为column列，每列unit_nr个字符数据
	 */
	QString toHexString(const QByteArray& ba,int unit_nr=8,int column=3);
	QString toBinString(const QByteArray& ba,int unit_nr=2,int column=4);
	QString toBinString(const unsigned char* data,int size,int unit_nr=2,int column=4);
	int getToBinStringColumnNr(int width,const QFontMetrics& fm,int unit_nr);
	/*
	 * 取文件的绝对路径，递归子目录
	 */
	QStringList filePathListInDir(const QString& dir_path,const QStringList& filter=QStringList());
	/*
	 * 取文件的基本名，递归子目录
	 */
	QStringList fileListInDir(const QString& dir_path,const QStringList& filter=QStringList());
	/*
	 * 取文件名，递归子目录
	 */
	QStringList fileNameListInDir(const QString& dir_path,const QStringList& filter=QStringList());

	QString dataSizeToStringByte(unsigned s);
	QString dataSizeToStringBits(unsigned s);
	/*
	 * name_array[] = {G,M,K,B};
	 */
	QString dataSizeToString(unsigned s,const QString name_array[]);
	/*
	 * 对名字大小写不敏感
	 */
	bool getPluginPathByName(const QString& name,const QString& dir_path,QString* path);
	/*
	 * 删除文件夹dir_path中满足条件filter(path)==true的文件
	 */
	bool removeFileIf(const QString& dir_path,const std::function<bool(const QString&)>& filter);
	template<class Filter>
		inline bool removeFileBefore(const QString& dir_path,const QDateTime& d,const Filter& filter) {
			return removeFileIf(dir_path,[&d,&filter](const QString& file_path) {
					if(QFileInfo(file_path).lastModified()>=d) return false;
					return filter(file_path);
					});
		}
	inline bool removeFileBefore(const QString& dir_path,const QDateTime& d) {
		return removeFileIf(dir_path,[&d](const QString& file_path) {
				if(QFileInfo(file_path).lastModified()>=d) return false;
				return true;
				});
	}
	/*
	 * 删除文件夹dir_path中满足条件filter(path)==true的文件
	 */
	/*
	 * 将数据ba使用code编码进行解码
	 */
	QByteArray transDecoding(QByteArray& ba,const QByteArray& code);
	QByteArray transDecoding(const char* str,const QByteArray& code);
	/*
	 * 检查文件路径，如果文件中有以下字符进行相应的处理：
	 * ~:修改为主目录路径
	 */
	bool checkPath(QString* path);
	int getToHexStringColumnNr(int width,const QFontMetrics& fm,int unit_nr);
	bool rmdir(const QString& dir);
	/*
	 * 拷贝一个文件或目录
	 * 源可以是Qt资源文件
	 */
	bool copy(const QString& src, const QString& dst);
	void assign(char* data,const QString& str,size_t size=0);
}
inline std::ostream& operator<<(std::ostream& s,const QString& data)
{
	s<<data.toUtf8().data();
	return s;
}
inline std::istream& operator>>(std::istream& s,QString& data)
{
	std::string str;
	s>>str;
	data = str.c_str();
	return s;
}
inline std::string operator+(const std::string& v0, const QString& v1)
{
	return v0+v1.toUtf8().data();
}
inline QString operator+(const QString& v0, const std::string& v1)
{
	return v0+v1.c_str();
}
