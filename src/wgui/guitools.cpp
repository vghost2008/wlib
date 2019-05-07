#include <guitools.h>
namespace WGui
{
	QString getPinYingKey(const QString& text,const std::map<char,QString>& dataset)
	{
		QString res;

		for(auto i=0; i<text.size(); ++i) {

			const auto c = text[i];
			const auto latin = c.toLatin1();

			if( ((latin>= 'a') && (latin<='z'))
				|| ((latin>= 'A') && (latin<='Z'))
				|| c.isDigit()) {
				res.append(c);
			} else {
				for(auto& p:dataset) {
					if(p.second.contains(c))
						res.append(p.first);
				}
			}
		}
		return res.toLower();
	}
}
