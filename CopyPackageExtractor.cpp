#include "dpkg_addon.h"

class CopyPackageExtractor : public DeltaPackageExtractor {
public:
	virtual const char* Name() const { return "CopyPackageExtractor"; }
	virtual uint32 Type() const { return 'COPY'; }
	
	virtual void Extract(DeltaPackageEntryInfo* info, BPositionIO* ActualFile,
		BPositionIO* DeltaPackage, BPositionIO* Result) {
			DeltaPackage->Seek(0, SEEK_SET);
			void* data = malloc(sizeof(uint8)*1024);
			ssize_t size = 0;
			while((size = DeltaPackage->Read(data, 1024)) != 0) {
				Result->Write(data, size);
			}
	}
};

DeltaPackageExtractor* CreateCopyPackageExtractor() {
	return new CopyPackageExtractor();
}
