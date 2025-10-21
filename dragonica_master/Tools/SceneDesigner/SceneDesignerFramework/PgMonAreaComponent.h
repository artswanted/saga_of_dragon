//#pragma once
//class PgMonAreaComponent : public NiSceneGraphComponent
//{
//public:
//    typedef enum ePGMonAreaType
//    {
//		Rectangle,
//		Circle,
//	}EPGMonAreaType;
//
//protected:
//	PgMonAreaComponent(void);
//	MEntity* AddNewTarget(NiPoint3 kPoint, NiPoint3 kTranslate);
//	MEntity* AddNewTarget(float fX, float fY, float fZ, NiPoint3 kTranslate);
//
//public:
//	PgMonAreaComponent(NiEntityPropertyInterface* pkStartEntity, NiEntityPropertyInterface* pkEndEntity, bool bIsRectangle);
//	PgMonAreaComponent(NiPoint3 kPoint1, NiPoint3 kPoint2, bool bIsRectangle);
//	virtual ~PgMonAreaComponent(void);
//	void CreateTriStrips(NiEntityPropertyInterface* pkStartEntity, NiEntityPropertyInterface* pkEndEntity);
//	void CreateTriStrips(NiPoint3 kPoint1, NiPoint3 kPoint2);
//	void CreateCircle(NiEntityPropertyInterface* pkStartEntity, NiEntityPropertyInterface* pkEndEntity);
//	void CreateCircle(NiPoint3 kPoint1, NiPoint3 kPoint2);
//	void RevertPoint();
//	void ExtractPoint(MEntity*& pkOutEntity1, MEntity*& pkOutEntity2);
//	NiPoint3 GetNormalVec();
//
//	EPGMonAreaType GetMonAreaType() const { return m_eMonAreaType; };
//	bool IsRevert() const { return m_bRevertPoint; };
//
//public:
//	// override
//    virtual void Update(NiEntityPropertyInterface* pkParentEntity,
//        float fTime, NiEntityErrorInterface* pkErrors,
//		NiExternalAssetManager* pkAssetManager);
//    virtual void BuildVisibleSet(NiEntityRenderingContext* pkRenderingContext,
//        NiEntityErrorInterface* pkErrors);
//    virtual NiFixedString GetClassName() const;
//    virtual NiFixedString GetName() const;
//
//public:
//	NiNode* m_pkSceneRoot;
//	// case Rectangle
//	NiTriStrips* m_pkTriStrips;	// 사각형 Obj
//	NiTriStrips* m_pkTriStrips2; // 반대면 보여주는 녀석.
//	// case Circle
//	NiTriStrips* m_pkCircle;
//	float m_fRadius;
//	// Default
//	// 이 값을 참고로 물체를 만든다.
//	NiEntityPropertyInterface* m_pkStartEntity;
//	NiEntityPropertyInterface* m_pkEndEntity; // 이 값을 참고로 사각형을 만든다.
//	NiPoint3 m_akOldPoint[2];
//
//protected:
//	EPGMonAreaType m_eMonAreaType;
//	bool m_bInitUpdated;
//	NiPoint3 m_kInitPoint;
//	bool m_bRevertPoint;
//};
