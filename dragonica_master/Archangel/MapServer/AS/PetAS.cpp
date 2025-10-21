#include "stdafx.h"
#include "ActorEventSystem.h"
#include "PgAction_Pet.h"
#include "PgGround.h"

static void PT_C_M_REQ_PET_RENAME__AES(AES::EVENT_RECIVER pkUnit, AES::EVENT_BODY pkBody, PgGround* pkGround)
{
    SItemPos kItemPos;
	std::wstring wstrName;
	pkBody->Pop( kItemPos );
	pkBody->Pop( wstrName );
    SItemPos kRenameItemPos;

	if ( !pkBody->Pop( kRenameItemPos ) )
        return;

	PgActPet_Rename_ForItem kAction( pkGround, kItemPos, kRenameItemPos );
	kAction.DoAction( pkUnit, wstrName );
}
AES_REGISTER_TYPED_SYSTEM(PT_C_M_REQ_PET_RENAME);
