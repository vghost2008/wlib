/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
_Pragma("once")
#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <QItemDelegate>
#include <memory>
#include <basexmldata.h>
#include "xmlviewdelegateimp.h"
/*
 * 视图编辑代理
 */
class WXmlView;
class WXmlItem;
class WXmlGroup;
class XmlBaseViewDelegate:public QItemDelegate
{
	public:
		XmlBaseViewDelegate(WXmlView* view);
	protected:
		bool      use_soft_keyboard_;
		WXmlView *view_;                //引用关系
		std::unique_ptr<WNormalItemDelegate::AbstractItemDelegate> item_delegates_[ItemType::IT_NR];
		friend WNormalItemDelegate::AbstractItemDelegate;
	protected:
		WXmlGroup* getGroup(const QModelIndex& index)const;
		WXmlItem* getItem(const QModelIndex& index)const;
};
class NormalXmlViewDelegate:public XmlBaseViewDelegate
{
	public:
		NormalXmlViewDelegate(WXmlView* view);
		~NormalXmlViewDelegate();
	private:
		virtual QWidget *createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index)const;
		virtual void	setEditorData(QWidget* editor, const QModelIndex& index )const;
		virtual void	setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex & index)const;
		virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};
