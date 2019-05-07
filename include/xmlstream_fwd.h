/********************************************************************************
 *   License     : 
 *   Author      : WangJie bluetornado@zju.edu.cn
 *   Description : 
 ********************************************************************************/

_Pragma("once")
#ifndef Q_MOC_RUN
#include <boost/property_tree/ptree.hpp>
#endif
#include "xmldata.h"
#include <QString>

#define     XML_ATTR         "<xmlattr>"
#define     ROOT_TAGNAME     "ConfigL"       //根标签
/*
 * xml attr
 */
#define     CONFIGURE_ID_TAGNAME           "id" 
#define     GROUPID_TAGNAME                "id"
#define     ITEMID_TAGNAME                 "id"
#define     CONFIGURE_NAME_TAGNAME         "name"
#define     GROUPNAME_TAGNAME              "name"
#define     ITEMNAME_TAGNAME               "name"
#define     USER_DATA_NAME_TAGNAME         "name"
#define     ITEMTYPE_TAGNAME               "type"
#define     CONFIGURE_VERSION_TAGNAME      "version"
#define     CURRENTVALUE_TAGNAME           "value"
#define     MINVALUE_TAGNAME               "min"
#define     MAXVALUE_TAGNAME               "max"
#define     GROUP_REPEAT_TAGNAME           "Repeat"  //对一组数据进行重复，重复次数通过属性value指定
#define     REPEAT_TAGNAME                 "repeat" //repeat="10",item名字可以用{},{0.5},{0.5,0.6}这种数字模式
#define     REPEAT_ID_TAGNAME              "repeat_id" //指定repeat id的赋值方式，不指定自动递增，指定后以repeat_index*a+b的方式赋值
#define     REINIT_ID                      "reinit_id"  //true,false
/*
 * 标签
 */
#define     GROUP_TAGNAME          "Group"
#define     ITEM_TAGNAME           "Item"
#define     ATTR_TAGNAME           "Attr"
#define     ITEMDESC_TAGNAME       "ItemDesc"
#define     DEFAULT_VALUE          "default_value"
#define     LISTVALUES_TAGNAME     "ListValues"
#define     USER_DATA_TAGNAME      "user_data"  //<user_data name="XXX">.....</user_data>,ConfigL,Group可也可以使用
/*
 * ATTR_TAGNAME内容
 * 用;分隔，值用=指定
 */
#define     DATASIZE_TAGNAME     "size"         //数据大小
#define     DECIMALS_TAGNAME     "decimals"     //小数后的位数,用于浮点数
#define     STEP_TAGNAME         "step"         //步进，用于整数
#define     MARGIN_TAGNAME       "margin"       //下一个距自己的距离,默认为0,如果小于零表示下一个直接换行
#define     WIDTH_TAGNAME        "width"        //自己占用的宽度
#define     UNIT_TAGNAME         "unit"         //显示的单位
#define     XA_READONLY          "readonly"     //控件是否只读,Group也可以使用，使用方法readonly="enalbe"
#define     XA_TRANS             "trans"        //显示数据时按(a*x+b)变换,存数据时按((y-b)/a)变换
#define     XA_HIDE              "hide"         //不显示数据
/*
 * 与旧格式兼容
 */
#define     OLD_ROOT_TAGNAME               "SystemConfigure"
#define     OLD_CONFIGURE_ID_TAGNAME       "SystemConfigureID"
#define     OLD_CONFIGURE_NAME_TAGNAME     "SystemConfigureName"
#define     OLD_DATAFLAG_TAGNAME           "DataFlag"
#define     OLD_GROUPFLAG_TAGNAME          "GroupFlag"
#define     OLD_GROUP_TAGNAME              "Group"
#define     OLD_GROUPID_TAGNAME            "GroupID"
#define     OLD_GROUPNAME_TAGNAME          "GroupName"
#define     OLD_ITEM_TAGNAME               "Item"
#define     OLD_ITEMNAME_TAGNAME           "ItemName"
#define     OLD_ITEMID_TAGNAME             "ItemID"
#define     OLD_ITEMDESC_TAGNAME           "ItemDesc"
#define     OLD_CURRENTVALUE_TAGNAME       "CurrentValue"
#define     OLD_DEFAULT_VALUE              "DefaultValue"
#define     OLD_LISTVALUES_TAGNAME         "ListValues"
#define     OLD_LISTVALUE_TAGNAME          "ListValue"
#define     OLD_ITEMTYPE_TAGNAME           "ItemType"
#define     OLD_DATASIZE_TAGNAME           "DataSize"
#define     OLD_MINVALUE_TAGNAME           "MinValue"
#define     OLD_MAXVALUE_TAGNAME           "MaxValue"
#define     OLD_DECIMALS_TAGNAME           "Decimals"
#define     OLD_STEP_TAGNAME               "Step"

class QString;
using XMLTree=boost::property_tree::ptree ;
using ErrorHandler= void (const QString,const QString,WXmlItem* item);
using ProcessUserDataHandler= void (const WXmlItem& item,XMLTree* tree);
