#ifndef CUSTOMDATAROLES_H
#define CUSTOMDATAROLES_H

#include <Qt>

namespace DataRoles{
	/** Custom data roles */
	enum CustomDataRoles{
		/** Role for text to display in progressbar
		 * when using ProgressViewDelegat
		 */
		ProgressText = Qt::UserRole,
	};
}

#endif // CUSTOMDATAROLES_H
