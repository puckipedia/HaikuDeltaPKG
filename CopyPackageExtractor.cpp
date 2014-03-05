#include "dpkg_addon.h"

#include <stdlib.h>
#include <stdio.h>

class CopyPackageExtractor : public DeltaPackageExtractor {
public:
	virtual const char* Name() const { return "CopyPackageExtractor"; }
	virtual uint32 Type() const { return 'COPY'; }
	
	virtual void Extract(DeltaPackageEntryInfo* info, BPositionIO* ActualFile,
		BPositionIO* DeltaPackage, BPositionIO* Result) {
			DeltaPackage->Seek(0, SEEK_SET);
			printf("Seeked to start\n");
			void* data = malloc(sizeof(uint8)*1024);
			printf("Malloced %d bytes of mem\n", sizeof(uint8)*1024);
			ssize_t size = 0;
			while((size = DeltaPackage->Read(data, 1024)) != 0) {
				printf("Read %d bytes of data\n", size);
				Result->Write(data, size);
			}
			free(data);
	}
};

DeltaPackageExtractor* CreateCopyPackageExtractor() {
	return new CopyPackageExtractor();
}
