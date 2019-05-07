/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <xmldata.h>
#include <memory>
#include <wgui/guiedit.h>
#include <wgui/guibutton.h>

class AbstractXMLWidgetVDImp:public QObject
{
	Q_OBJECT
	public:
		AbstractXMLWidgetVDImp(std::shared_ptr<WXmlItem>& item,int gid);
		virtual ~AbstractXMLWidgetVDImp();
		virtual QWidget* createWidget(QWidget* parent)=0;
		virtual void setEditorData()=0;
		virtual void setModelData()=0;
		inline std::shared_ptr<WXmlItem>& item(){ return item_; }
		inline const std::shared_ptr<WXmlItem>& item()const { return item_; }
		inline int gid()const { return gid_; }
		inline int iid()const { return item_->id(); }
		void valueChanged();
	private:
		std::shared_ptr<WXmlItem> item_;
		int                       gid_      = 0;
		volatile bool             sig_send_ = false;
	signals:
		void explain();
		void sigValueChanged(int gid, int iid);
};
class StringXMLWidgetVDImp:public AbstractXMLWidgetVDImp
{
	public:
		using AbstractXMLWidgetVDImp::AbstractXMLWidgetVDImp;
		virtual QWidget* createWidget(QWidget* parent) override;
		virtual void setEditorData() override;
		virtual void setModelData() override;
	private:
		QLineEdit editor_;
};
class IntXMLWidgetVDImp:public AbstractXMLWidgetVDImp
{
	public:
		using AbstractXMLWidgetVDImp::AbstractXMLWidgetVDImp;
		virtual QWidget* createWidget(QWidget* parent) override;
		virtual void setEditorData() override;
		virtual void setModelData() override;
	private:
		QSpinBox editor_;
};
class UIntXMLWidgetVDImp:public AbstractXMLWidgetVDImp
{
	public:
		using AbstractXMLWidgetVDImp::AbstractXMLWidgetVDImp;
		virtual QWidget* createWidget(QWidget* parent) override;
		virtual void setEditorData() override;
		virtual void setModelData() override;
	private:
		WUIntSpinBox editor_;
};
class FloatXMLWidgetVDImp:public AbstractXMLWidgetVDImp
{
	public:
		using AbstractXMLWidgetVDImp::AbstractXMLWidgetVDImp;
		virtual QWidget* createWidget(QWidget* parent) override;
		virtual void setEditorData() override;
		virtual void setModelData() override;
	private:
		QDoubleSpinBox editor_;
};
class IPXMLWidgetVDImp:public AbstractXMLWidgetVDImp
{
	public:
		using AbstractXMLWidgetVDImp::AbstractXMLWidgetVDImp;
		virtual QWidget* createWidget(QWidget* parent) override;
		virtual void setEditorData() override;
		virtual void setModelData() override;
	private:
		QLineEdit editor_;
};
class HexXMLWidgetVDImp:public AbstractXMLWidgetVDImp
{
	public:
		using AbstractXMLWidgetVDImp::AbstractXMLWidgetVDImp;
		virtual QWidget* createWidget(QWidget* parent) override;
		virtual void setEditorData() override;
		virtual void setModelData() override;
	private:
		WHexSpinBox editor_;
};
class ListXMLWidgetVDImp:public AbstractXMLWidgetVDImp
{
	public:
		using AbstractXMLWidgetVDImp::AbstractXMLWidgetVDImp;
		virtual QWidget* createWidget(QWidget* parent) override;
		virtual void setEditorData() override;
		virtual void setModelData() override;
	private:
		QComboBox editor_;
};
class DListXMLWidgetVDImp:public AbstractXMLWidgetVDImp
{
	public:
		using AbstractXMLWidgetVDImp::AbstractXMLWidgetVDImp;
		virtual QWidget* createWidget(QWidget* parent) override;
		virtual void setEditorData() override;
		virtual void setModelData() override;
	private:
		QComboBox editor_;
};
class EnumXMLWidgetVDImp:public AbstractXMLWidgetVDImp
{
	public:
		using AbstractXMLWidgetVDImp::AbstractXMLWidgetVDImp;
		virtual QWidget* createWidget(QWidget* parent) override;
		virtual void setEditorData() override;
		virtual void setModelData() override;
	private:
		QComboBox editor_;
};
class DirXMLWidgetVDImp:public AbstractXMLWidgetVDImp
{
	public:
		using AbstractXMLWidgetVDImp::AbstractXMLWidgetVDImp;
		virtual QWidget* createWidget(QWidget* parent) override;
		virtual void setEditorData() override;
		virtual void setModelData() override;
	private:
		WDirEdit editor_;
};
class FileXMLWidgetVDImp:public AbstractXMLWidgetVDImp
{
	public:
		using AbstractXMLWidgetVDImp::AbstractXMLWidgetVDImp;
		virtual QWidget* createWidget(QWidget* parent) override;
		virtual void setEditorData() override;
		virtual void setModelData() override;
	private:
		WFileEdit editor_;
};
class TimeXMLWidgetVDImp:public AbstractXMLWidgetVDImp
{
	public:
		using AbstractXMLWidgetVDImp::AbstractXMLWidgetVDImp;
		virtual QWidget* createWidget(QWidget* parent) override;
		virtual void setEditorData() override;
		virtual void setModelData() override;
	private:
		QTimeEdit editor_;
};
class ColorXMLWidgetVDImp:public AbstractXMLWidgetVDImp
{
	public:
		using AbstractXMLWidgetVDImp::AbstractXMLWidgetVDImp;
		virtual QWidget* createWidget(QWidget* parent) override;
		virtual void setEditorData() override;
		virtual void setModelData() override;
	private:
		WColorEdit editor_;
};
class BoolXMLWidgetVDImp:public AbstractXMLWidgetVDImp
{
	public:
		using AbstractXMLWidgetVDImp::AbstractXMLWidgetVDImp;
		virtual QWidget* createWidget(QWidget* parent) override;
		virtual void setEditorData() override;
		virtual void setModelData() override;
	private:
		QCheckBox editor_;
};
class StatusXMLWidgetVDImp:public AbstractXMLWidgetVDImp
{
	public:
		StatusXMLWidgetVDImp(std::shared_ptr<WXmlItem>& item,int gid);
		virtual QWidget* createWidget(QWidget* parent) override;
		virtual void setEditorData() override;
		virtual void setModelData() override;
	private:
		WStateButton editor_;
};
