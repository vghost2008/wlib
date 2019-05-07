/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include "xmlviewdelegateimp.h"
namespace WSoftKeyItemDelegate
{
using WNormalItemDelegate::AbstractItemDelegate;
using WNormalItemDelegate::ListItemDelegate;
using WNormalItemDelegate::DListItemDelegate;
using WNormalItemDelegate::FileItemDelegate;
using WNormalItemDelegate::DirItemDelegate;
using WNormalItemDelegate::TimeItemDelegate;
using WNormalItemDelegate::ColorItemDelegate;
using WNormalItemDelegate::BoolItemDelegate;
using WNormalItemDelegate::EnumItemDelegate;
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
		virtual QWidget* createEditor(WXmlItem* item,QWidget* parent,const QStyleOptionViewItem& option,const QModelIndex& index)const override;
		virtual void setEditorData(WXmlItem* item,QWidget* editor,const QModelIndex& index)const override;
		virtual void setModelData(WXmlItem* item,QWidget* editor, QAbstractItemModel* model, const QModelIndex & index)const override;
};
class UIntItemDelegate:public AbstractItemDelegate
{
	public:
		using AbstractItemDelegate::AbstractItemDelegate;
		virtual QWidget* createEditor(WXmlItem* item,QWidget* parent,const QStyleOptionViewItem& option,const QModelIndex& index)const ;
		virtual void setEditorData(WXmlItem* item,QWidget* editor,const QModelIndex& index)const ;
		virtual void setModelData(WXmlItem* item,QWidget* editor, QAbstractItemModel* model, const QModelIndex & index)const;
};
class FloatItemDelegate:public AbstractItemDelegate
{
	public:
		using AbstractItemDelegate::AbstractItemDelegate;
		virtual QWidget* createEditor(WXmlItem* item,QWidget* parent,const QStyleOptionViewItem& option,const QModelIndex& index)const ;
		virtual void setEditorData(WXmlItem* item,QWidget* editor,const QModelIndex& index)const ;
		virtual void setModelData(WXmlItem* item,QWidget* editor, QAbstractItemModel* model, const QModelIndex & index)const;
};
class IPItemDelegate:public AbstractItemDelegate
{
	public:
		using AbstractItemDelegate::AbstractItemDelegate;
		virtual QWidget* createEditor(WXmlItem* item,QWidget* parent,const QStyleOptionViewItem& option,const QModelIndex& index)const ;
		virtual void setEditorData(WXmlItem* item,QWidget* editor,const QModelIndex& index)const ;
		virtual void setModelData(WXmlItem* item,QWidget* editor, QAbstractItemModel* model, const QModelIndex & index)const;
};
class HexItemDelegate:public AbstractItemDelegate
{
	public:
		using AbstractItemDelegate::AbstractItemDelegate;
		virtual QWidget* createEditor(WXmlItem* item,QWidget* parent,const QStyleOptionViewItem& option,const QModelIndex& index)const ;
		virtual void setEditorData(WXmlItem* item,QWidget* editor,const QModelIndex& index)const ;
		virtual void setModelData(WXmlItem* item,QWidget* editor, QAbstractItemModel* model, const QModelIndex & index)const;
};
} //end namespace
