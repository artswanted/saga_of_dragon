#ifndef CENTER_CENTERSERVER_NETWORK_PGUSERWRAPPER_H
#define CENTER_CENTERSERVER_NETWORK_PGUSERWRAPPER_H

extern HRESULT GetPlayerByGuid(BM::GUID const &rkGuid, bool const bIsMemberGuid, SContentsUser &rkOut, bool const bFindOtherChannel = false);
extern HRESULT GetPlayerByName(std::wstring const &rkName, SContentsUser &rkOut, bool const bFindOtherChannel = false);
extern HRESULT WritePlayerInfoToPacket_ByGuid(BM::GUID const &rkGuid, bool const bIsMemberGuid, BM::Stream &rkPacket, bool const bFindOtherChannel = false);
extern HRESULT WritePlayerInfoToPacket_ByName(std::wstring const &rkName, BM::Stream &rkPacket, bool const bFindOtherChannel = false);
extern HRESULT GetCharacterNameByGuid(BM::GUID const &rkGuid, bool const bIsMemberGuid, std::wstring &rkOut, bool const bFindOtherChannel = false);
extern HRESULT GetCharGuidToMembGuid(VEC_GUID const &rkIn, VEC_GUID &rkOut, bool const bFindOtherChannel = false);

#endif // CENTER_CENTERSERVER_NETWORK_PGUSERWRAPPER_H