#ifndef _FBXEXPORTER_EXPORTER_DEFINES_H_
#define _FBXEXPORTER_EXPORTER_DEFINES_H_

namespace fbx_exporter
{
	// Indicates how data should be used after being read from file.
	enum struct FileReadMode
	{
		EXTRACT = 0  // Read and store data.
		, EXPORT  // Read, store, and export data.
	};

}

#endif // _FBXEXPORTER_EXPORTER_DEFINES_H_