/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/
 
_Pragma("once")
#include <QGraphicsScene>
#include <QtCore>
#include <unordered_map>
#include "wgraph_tree.h"
#include "wgraph_fwd.h"

class WGraphItem;
class WGraphLine;
class WGraphScene:public QGraphicsScene
{
	public:
		WGraphScene();
		~WGraphScene();
		WGraphTree& graphTree() { return graph_tree_; }
		WGraphTree& graphTree()const { return graph_tree_; }
		void updateGraphTree()const;
		void updateGraphTree(int id, const QRectF& v)const;
		void updateGraphTree(const WGraphItem& item)const;
		void graphTreeRemove(int id)const;
		void graphTreeInsert(int id,const QRectF& v)const;
		void clearGraphTree()const;
		bool collidingNode(const WGraphLine& line)const;
		/*
		 * 与item有交叉的线的列表
		 */
		std::vector<WGraphLine*> collidingLines(const WGraphItem& item);
		inline void set_item_connection(const std::map<int,std::vector<int>>& data) { item_connection_ = data; }
		inline void set_line_type(WGraphLineType type) { line_type_ = type; }
	public:
		std::shared_ptr<WGraphItem> getItemById(int id)noexcept;
	public:
		void addItem(std::shared_ptr<WGraphItem> item);
		void removeAllItems();
		/*
		 * 用于生成item之间的连接线
		 * 前置条件：
		 * 所有item的位置已经确定
		 */
		void addLinkLine();
		void addLinkLine(const std::map<int,std::vector<int>>& item_connection);
		/*
		 * 用于设置item位置
		 * 前置条件所有的item已经通过WGraphScene::addItem添加到场景中,item_connection_已经设置
		 * min_length:item之间的最小距离,用于粗调
		 * view_size:场景大小
		 * min_margin:item之间的最小距离,用于微调
		 */
		virtual void initItemPosition(float min_length=20,const QSizeF& view_size=QSizeF(800,600),const QSizeF& min_margin=QSizeF(20,20));
	protected:
		void getItemPosition(const QSizeF& view_size,float min_length,WGraph::Graph& g,WGraph::PositionMap& position);
		inline WGraphLineType line_type()const { return line_type_; }
	private:
		mutable WGraphTree graph_tree_;
		QPointF            current_pos_;
		std::unordered_map<int,std::shared_ptr<WGraphItem>> id_to_item_map_;
		/*
		 * first线的起始item ID, second为线的结束端item ID
		 */
		std::map<int,std::vector<int>> item_connection_;
		WGraphLineType line_type_ = WGLT_FREE;
		friend class WGraphItem;
		friend WGraphLine;
	private:
		WGraphItem& getCurrentItem()noexcept(false);
		WGraphLine& getCurrentLine()noexcept(false);
		void checkLines();
		void adjustItemPosition(float x_step, float y_step);
		bool hasOverlap(QGraphicsItem* item);
		void addItem(QGraphicsItem* item);
};
