#ifndef MAP_MAPSERVER_OBJECT_OPENMARKET_PGLOCALVENDORMGR_H
#define MAP_MAPSERVER_OBJECT_OPENMARKET_PGLOCALVENDORMGR_H
//
class PgLocalVendorMgrImpl
{
public:
	PgLocalVendorMgrImpl();
	virtual ~PgLocalVendorMgrImpl();

	void Clear();
	bool ReadToPacketVendorName(BM::GUID const& rkCharGuid, BM::Stream& rkPacket);
	bool Create(BM::GUID const& rkCharGuid, std::wstring const& rkNewVendorTitle );
	bool Rename(BM::GUID const& rkCharGuid, std::wstring const& rkNewVendorTitle );
	virtual bool Delete(BM::GUID const& rkCharGuid);

private:
	typedef std::map<BM::GUID, std::wstring> CONT_VENDOR_LIST;
	CONT_VENDOR_LIST m_kContVendorList;

};

//
class PgLocalVendorMgr
	: TWrapper< PgLocalVendorMgrImpl >
{
public:
	PgLocalVendorMgr();
	virtual ~PgLocalVendorMgr();

	void Clear();
//protected:
	bool ReadToPacketVendorName(BM::GUID const& rkCharGuid, BM::Stream& rkPacket);
	bool Create(BM::GUID const& rkCharGuid, std::wstring const& rkNewVendorTitle);
	bool Rename(BM::GUID const& rkCharGuid, std::wstring const& rkNewVendorTitle);
	bool Delete(BM::GUID const& rkCharGuid);
};

#endif // MAP_MAPSERVER_OBJECT_OPENMARKET_PGLOCALVENDORMGR_H