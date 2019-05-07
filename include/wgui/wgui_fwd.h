/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <QString>
#include <QFile>
#include <QColor>

enum GraphicsPlace {
	G_ALL     ,  
	G_LEFT    ,  
	G_MID     ,  
	G_RIGHT   ,  
	G_TOP     ,  
	G_BOTTOM  ,  
};
enum WOrientation{
	Horizontal   ,   
	Vertical     ,   
};
enum WButtonState
{
	NORMAL    =   0x0000,
	HOVER     =   0x0001,
	PRESSED   =   0x0002,
};
struct GradientNode
{
	float position;
	QColor color;
};
inline QString imgP(const QString& file_name,const QString& suffix=".png") {
	const QString image_path = QString(":/images/")+file_name+suffix;
	if(!QFile::exists(image_path)) {
		qDebug(QString(QString("Image file dosen't exist:")+image_path).toUtf8().data());
	}
	return image_path;
}
