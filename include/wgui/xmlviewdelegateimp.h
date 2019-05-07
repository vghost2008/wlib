/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <QtCore>
#include <QtGui>
#include <QtWidgets>
class WXmlView;
class WXmlItem;
class XmlBaseViewDelegate;
namespace WNormalItemDelegate {
class AbstractItemDelegate
{
	public:
		AbstractItemDelegate(XmlBaseViewDelegate* view_delegate):view_delegate_(view_delegate){}
		virtual ~AbstractItemDelegate();
		virtual QWidget* createEditor(WXmlItem* item,QWidget* parent,const QStyleOptionViewItem& option,const QModelIndex& index)const = 0;
		virtual void setEditorData(WXmlItem* item,QWidget* editor,const QModelIndex& index)const =0;
		virtual void	setModelData(WXmlItem* item,QWidget* editor, QAbstractItemModel* model, const QModelIndex & index)const=0;
		virtual bool paint(WXmlItem* item,QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	protected:
		WXmlView* view()const;
	protected:
		mutable XmlBaseViewDelegate* view_delegate_ = nullptr; //引用
};
class StringItemDelegate:public AbstractItemDelegate
{
	public:
		using AbstractItemDelegate::AbstractItemDelegate;
		virtual QWidget* createEditor(WXmlItem* item,QWidget* parent,const QStyleOptionViewItem& option,const QModelIndex& index)const ;
		virtual void setEditorData(WXmlItem* item,QWidget* editor,const QModelIndex& index)const ;
		virtual void setModelData(WXmlItem* item,QWidget* editor, QAbstractItemModel* model, const QModelIndex & index)const;
};
class IntItemDelegate:public AbstractItemDelegate
{
	public:
		using AbstractItemDelegate::AbstractItemDelegate;
		virtual QWidget* createEditor(WXmlItem* item,QWidget* parent,const QStyleOptionViewItem& option,const QModelIndex& index)const ;
		virtual void setEditorData(WXmlItem* item,QWidget* editor,const QModelIndex& index)const ;
		virtual void setModelData(WXmlItem* item,QWidget* editor, QAbstractItemModel* model, const QModelIndex & index)const;
};
class UIntItemDelegate:public AbstractItemDelegate
{
	public:
		using AbstractItemDelegate::AbstractItemDelegate;
		virtual QWidget* createEditor(WXmlItem* item,QWidget* parent,const QStyleOptionViewItem& option,const QModelIndex& index)const ;
		virtual void setEditorData(WXmlItem* item,QWidget* editor,const QModelIndex& index)const ;
		virtual void	setModelData(WXmlItem* item,QWidget* editor, QAbstractItemModel* model, const QModelIndex & index)const;
};
class FloatItemDelegate:public AbstractItemDelegate
{
	public:
		using AbstractItemDelegate::AbstractItemDelegate;
		virtual QWidget* createEditor(WXmlItem* item,QWidget* parent,const QStyleOptionViewItem& option,const QModelIndex& index)const ;
		virtual void setEditorData(WXmlItem* item,QWidget* editor,const QModelIndex& index)const ;
		virtual void	setModelData(WXmlItem* item,QWidget* editor, QAbstractItemModel* model, const QModelIndex & index)const;
};
class IPItemDelegate:public AbstractItemDelegate
{
	public:
		using AbstractItemDelegate::AbstractItemDelegate;
		virtual QWidget* createEditor(WXmlItem* item,QWidget* parent,const QStyleOptionViewItem& option,const QModelIndex& index)const ;
		virtual void setEditorData(WXmlItem* item,QWidget* editor,const QModelIndex& index)const ;
		virtual void	setModelData(WXmlItem* item,QWidget* editor, QAbstractItemModel* model, const QModelIndex & index)const;
};
class HexItemDelegate:public AbstractItemDelegate
{
	public:
		using AbstractItemDelegate::AbstractItemDelegate;
		virtual QWidget* createEditor(WXmlItem* item,QWidget* parent,const QStyleOptionViewItem& option,const QModelIndex& index)const ;
		virtual void setEditorData(WXmlItem* item,QWidget* editor,const QModelIndex& index)const ;
		virtual void	setModelData(WXmlItem* item,QWidget* editor, QAbstractItemModel* model, const QModelIndex & index)const;
};
class ListItemDelegate:public AbstractItemDelegate
{
	public:
		using AbstractItemDelegate::AbstractItemDelegate;
		virtual QWidget* createEditor(WXmlItem* item,QWidget* parent,const QStyleOptionViewItem& option,const QModelIndex& index)const ;
		virtual void setEditorData(WXmlItem* item,QWidget* editor,const QModelIndex& index)const ;
		virtual void	setModelData(WXmlItem* item,QWidget* editor, QAbstractItemModel* model, const QModelIndex & index)const;
};
class EnumItemDelegate:public AbstractItemDelegate
{
	public:
		using AbstractItemDelegate::AbstractItemDelegate;
		virtual QWidget* createEditor(WXmlItem* item,QWidget* parent,const QStyleOptionViewItem& option,const QModelIndex& index)const ;
		virtual void setEditorData(WXmlItem* item,QWidget* editor,const QModelIndex& index)const ;
		virtual void	setModelData(WXmlItem* item,QWidget* editor, QAbstractItemModel* model, const QModelIndex & index)const;
};
class BoolItemDelegate:public AbstractItemDelegate
{
	public:
		using AbstractItemDelegate::AbstractItemDelegate;
		virtual QWidget* createEditor(WXmlItem* item,QWidget* parent,const QStyleOptionViewItem& option,const QModelIndex& index)const ;
		virtual void setEditorData(WXmlItem* item,QWidget* editor,const QModelIndex& index)const ;
		virtual void	setModelData(WXmlItem* item,QWidget* editor, QAbstractItemModel* model, const QModelIndex & index)const;
};
class DListItemDelegate:public AbstractItemDelegate
{
	public:
		using AbstractItemDelegate::AbstractItemDelegate;
		virtual QWidget* createEditor(WXmlItem* item,QWidget* parent,const QStyleOptionViewItem& option,const QModelIndex& index)const ;
		virtual void setEditorData(WXmlItem* item,QWidget* editor,const QModelIndex& index)const ;
		virtual void	setModelData(WXmlItem* item,QWidget* editor, QAbstractItemModel* model, const QModelIndex & index)const;
};
class DirItemDelegate:public AbstractItemDelegate
{
	public:
		using AbstractItemDelegate::AbstractItemDelegate;
		virtual QWidget* createEditor(WXmlItem* item,QWidget* parent,const QStyleOptionViewItem& option,const QModelIndex& index)const ;
		virtual void setEditorData(WXmlItem* item,QWidget* editor,const QModelIndex& index)const ;
		virtual void	setModelData(WXmlItem* item,QWidget* editor, QAbstractItemModel* model, const QModelIndex & index)const;
};
class FileItemDelegate:public AbstractItemDelegate
{
	public:
		using AbstractItemDelegate::AbstractItemDelegate;
		virtual QWidget* createEditor(WXmlItem* item,QWidget* parent,const QStyleOptionViewItem& option,const QModelIndex& index)const ;
		virtual void setEditorData(WXmlItem* item,QWidget* editor,const QModelIndex& index)const ;
		virtual void	setModelData(WXmlItem* item,QWidget* editor, QAbstractItemModel* model, const QModelIndex & index)const;
};
class TimeItemDelegate:public AbstractItemDelegate
{
	public:
		using AbstractItemDelegate::AbstractItemDelegate;
		virtual QWidget* createEditor(WXmlItem* item,QWidget* parent,const QStyleOptionViewItem& option,const QModelIndex& index)const ;
		virtual void setEditorData(WXmlItem* item,QWidget* editor,const QModelIndex& index)const ;
		virtual void	setModelData(WXmlItem* item,QWidget* editor, QAbstractItemModel* model, const QModelIndex & index)const;
};
class ColorItemDelegate:public AbstractItemDelegate
{
	public:
		using AbstractItemDelegate::AbstractItemDelegate;
		virtual QWidget* createEditor(WXmlItem* item,QWidget* parent,const QStyleOptionViewItem& option,const QModelIndex& index)const ;
		virtual void setEditorData(WXmlItem* item,QWidget* editor,const QModelIndex& index)const ;
		virtual void	setModelData(WXmlItem* item,QWidget* editor, QAbstractItemModel* model, const QModelIndex & index)const;
		virtual bool paint(WXmlItem* item,QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};
}
