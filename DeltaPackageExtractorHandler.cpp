#include "DeltaPackageExtractorHandler.h"

#include <os/support/ByteOrder.h>

#include <string.h>

map<uint32, DeltaPackageExtractor*> DeltaPackageExtractorHandler::gDeltaFileHandlers;

DeltaPackageExtractorHandler::DeltaPackageExtractorHandler()
{
	fPath.SetTo("/");
}

void*
DeltaPackageExtractorHandler::GetData(BPackageData& data) {
	void* dat = malloc(data.Size());
	if(data.IsEncodedInline()) {
		memcpy(dat, static_cast<const void*>(data.InlineData()), data.Size());
		return dat;
	} else {
		fHeapReader->ReadData(data.Offset(), dat, data.Size());
		return dat;
	}
	free(dat);
	return NULL;
}


status_t
DeltaPackageExtractorHandler::HandleEntry(BPackageEntry* entry)
{
	this->fPath.Append(entry->Name());
	
	DeltaPackageEntryInfo* info = new DeltaPackageEntryInfo();
	
	info->fHandlerID = 'UNKN';
	info->fExtractedSize = entry->Data().Size();
	
	info->fStringPath = this->fPath.Path();
	
	entry->SetUserToken(info);
	
	return B_OK;
}


status_t
DeltaPackageExtractorHandler::HandleEntryAttribute(BPackageEntry* entry, BPackageEntryAttribute* attribute)
{
	DeltaPackageEntryInfo* info = static_cast<DeltaPackageEntryInfo*>(entry->UserToken());
	
	BString name = attribute->Name();

	if (name == "DPKG:HANDLER") {
		void* data = GetData(attribute->Data());
		if (data != NULL) {
			info->fHandlerID = *static_cast<int32*>(data);
			free(data);
		}
	}

	if (name == "DPKG:EXTRSIZE") {
		void* data = GetData(attribute->Data());
		if (data != NULL) {
			info->fExtractedSize = *static_cast<uint64*>(data);
			free(data);
		}
	}
	return B_OK;
}


status_t
DeltaPackageExtractorHandler::HandleEntryDone(BPackageEntry* entry)
{	
	DeltaPackageEntryInfo* info = static_cast<DeltaPackageEntryInfo*>(entry->UserToken());
	info->fPackageEntry = entry;
	
	int32 what = B_BENDIAN_TO_HOST_INT32(info->fHandlerID);
	
	this->fPath.GetParent(&this->fPath);
	
	fPackageEntries[info->fStringPath.String()] = info;
	
	printf("Entry %s done! %.4s %"B_PRIu64"\n", info->fStringPath.String(), (char*)&what, info->fExtractedSize);
	
	return B_OK;
}


status_t
DeltaPackageExtractorHandler::HandlePackageAttribute(const BPackageInfoAttributeValue& value)
{
	return B_OK;
}


void
DeltaPackageExtractorHandler::HandleErrorOccurred()
{
}
