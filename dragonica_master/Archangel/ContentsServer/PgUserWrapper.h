#ifndef CONTENTS_CONTENTSSERVER_NETWORK_PGUSERWRAPPER_H
#define CONTENTS_CONTENTSSERVER_NETWORK_PGUSERWRAPPER_H

//extern HRESULT GetPlayerByGuid(BM::GUID const &rkGuid, bool const bIsMemberGuid, SContentsUser &rkOut, bool const bFindOtherChannel = false);
extern HRESULT GetPlayerByGuid(BM::GUID const &rkGuid, bool const bIsMemberGuid, SContentsUser &rkOut);
//extern HRESULT GetPlayerByName(std::wstring const &rkName, SContentsUser &rkOut, bool const bFindOtherChannel = false);
extern HRESULT GetPlayerByName(std::wstring const &rkName, SContentsUser &rkOut);
//extern HRESULT WritePlayerInfoToPacket_ByGuid(BM::GUID const &rkGuid, bool const bIsMemberGuid, BM::Stream &rkPacket, bool const bFindOtherChannel = false);
extern HRESULT WritePlayerInfoToPacket_ByGuid(BM::GUID const &rkGuid, bool const bIsMemberGuid, BM::Stream &rkPacket);
//extern HRESULT WritePlayerInfoToPacket_ByName(std::wstring const &rkName, BM::Stream &rkPacket, bool const bFindOtherChannel = false);
extern HRESULT WritePlayerInfoToPacket_ByName(std::wstring const &rkName, BM::Stream &rkPacket);
//extern HRESULT GetCharacterNameByGuid(BM::GUID const &rkGuid, bool const bIsMemberGuid, std::wstring &rkOut, bool const bFindOtherChannel = false);
extern HRESULT GetCharacterNameByGuid(BM::GUID const &rkGuid, bool const bIsMemberGuid, std::wstring &rkOut);
//extern HRESULT GetCharGuidToMembGuid(VEC_GUID const &rkIn, VEC_GUID &rkOut, bool const bFindOtherChannel = false);
extern HRESULT GetCharGuidToMembGuid(VEC_GUID const &rkIn, VEC_GUID &rkOut);

#endif // CONTENTS_CONTENTSSERVER_NETWORK_PGUSERWRAPPER_H