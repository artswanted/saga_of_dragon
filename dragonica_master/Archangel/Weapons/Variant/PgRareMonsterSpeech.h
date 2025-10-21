#ifndef WEAPON_VARIANT_PG_RAREMONSTERSPEECH_H
#define WEAPON_VARIANT_PG_RAREMONSTERSPEECH_H

class PgRareMonsterSpeech
{
private:
	PgRareMonsterSpeech(){}
	~PgRareMonsterSpeech(){}

public:

	static bool GetSpeech(int const iTalkNo,int & iSpeechNo);
};

#endif // WEAPON_VARIANT_PG_RAREMONSTERSPEECH_H