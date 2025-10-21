using System;
using System.Diagnostics;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.Xml;
using System.Data.OleDb;
using System.Collections;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;
using Emergent.Gamebryo.SceneDesigner.Framework;

namespace Emergent.Gamebryo.SceneDesigner.GUI.ProjectG
{
    public partial class BreakObjectPanel : Form
    {
        #region Private Data
        private IUICommandService m_uiCommandService;
        //private ArrayList m_pmSettedMonsterList = new ArrayList();		  //중복이 있는 리스트
        private ArrayList m_pmMonsterBagControlList = new ArrayList();      // 몬스터 컨트롤 리스트
        //private ArrayList m_pmMonsterGuidListFromServer = new ArrayList();// 업데이트 할지 인서트 할지 정하기 위해 저장해두는 인덱스

        private string m_strXmlPath;
        private string m_strServer = "125.131.115.88,2433";
        private string m_strID = "sa";
        private string m_strPassword = "";
        private string m_strDatabase = "DR2_Def";
        private string m_strRegenTable = "TB_DefMapMonsterRegen";
        private string m_strBagControlTable = "TB_DefMonsterBagControl";
        private string m_strObjectBagTalbe = "TB_DefObjectBag";
        private string m_strObjectElementTable = "TB_DefObjectElement";
        // Xml 관리자
        private XmlManager m_XmlManager;
        #endregion

        public BreakObjectPanel()
        {
            InitializeComponent();

            UICommandService.BindCommands(this);

            LoadConfig();
            SaveConfig();

            tbElementNo.Text = "0";
            tbObjectNumber.Text = "0";
            tbPosX.Text = "0";
            tbPosY.Text = "0";
            tbPosZ.Text = "0";
            tbGroupName.Text = "";
            tbRegenTime.Text = "0";
            tbGroupNum.Text = "";
            labelDBConnectState.Text = "";
            tbMapNumber.Text = "";

            //test
            m_strXmlPath = "";
            tbXmlPath.Text = "";
            tbObjectNumber.Text = "";
        }

        private IUICommandService UICommandService
        {
            get
            {
                if (m_uiCommandService == null)
                {
                    m_uiCommandService =
                        ServiceProvider.Instance.GetService(
                        typeof(IUICommandService))
                        as IUICommandService;
                }
                return m_uiCommandService;
            }
        }

        private void LoadConfig()
        {
            if (MFramework.Instance.StreamManager.LoadConfig("config.txt"))
            {
                string strServer = MFramework.Instance.StreamManager.ServerIP;
                string strID = MFramework.Instance.StreamManager.ServerID;
                string strDatabase = MFramework.Instance.StreamManager.ServerDatabase;
                string strRegenTable = MFramework.Instance.StreamManager.ServerRegenTable;
				string strXmlPath = MFramework.Instance.StreamManager.XmlPath;

                if (strServer.Length > 0)
                    m_strServer = strServer;
                if (strID.Length > 0)
                    m_strID = strID;
                if (strDatabase.Length > 0)
                    m_strDatabase = strDatabase;
                if (strRegenTable.Length > 0)
                    m_strRegenTable = strRegenTable;
				if (strXmlPath.Length > 0)
					tbXmlPath.Text = strXmlPath;
            }
        }

        private void SaveConfig()
        {
            MFramework.Instance.StreamManager.ServerIP = m_strServer;
            MFramework.Instance.StreamManager.ServerID = m_strID;
            MFramework.Instance.StreamManager.ServerDatabase = m_strDatabase;
            MFramework.Instance.StreamManager.ServerRegenTable = m_strRegenTable;
			MFramework.Instance.StreamManager.XmlPath = tbXmlPath.Text;
            MFramework.Instance.StreamManager.SaveConfig("config.txt");
        }

        private string GetGroupNumToBagControl(int iParentBagNo)
        {
            for (int i = 0; i < m_pmMonsterBagControlList.Count; i++)
            {
                MonsterBagControl kData = (MonsterBagControl)m_pmMonsterBagControlList[i];
                if (kData.m_iParentBagNo == iParentBagNo)
                {
                    if (kData.m_iEasyBagNo != 0)
                    {
                        return "1";
                    }
                    else if (kData.m_iNormalBagNo != 0)
                    {
                        return "2";
                    }
                    else if (kData.m_iHardBagNo != 0)
                    {
                        return "3";
                    }
                    else if (kData.m_iUltraBagNo != 0)
                    {
                        return "4";
                    }
                    else if (kData.m_iLevel05Bag != 0)
                    {
                        return "5";
                    }
                    else if (kData.m_iLevel06Bag != 0)
                    {
                        return "6";
                    }
                }
            }

            return "0";
        }

        private void SyncObjectBag()
        {
            listObjectBag.Items.Clear();
            for(int nn=0; nn<MFramework.Instance.BreakObject.m_pmAllBagList.Count; ++nn)
            {
                ObjectBagData kData = (ObjectBagData)MFramework.Instance.BreakObject.m_pmAllBagList[nn];
                if (kData.m_nChangeFlag == 3)
                    continue;

                string strText = "Bag:" + kData.m_nBagNo + " Elements:" + " " + kData.m_nObjElement1 + " " + kData.m_nObjElement2
                 + " " + kData.m_nObjElement3 + " " + kData.m_nObjElement4 + " " + kData.m_nObjElement5
                 + "  " + kData.m_nObjElement6 + " " + kData.m_nObjElement7 + " " + kData.m_nObjElement8
                 + " " + kData.m_nObjElement9 + " " + kData.m_nObjElement10;

                listObjectBag.Items.Add(strText);
            }
        }

        private void SyncObjectElement()
        {
            listObjectElement.Items.Clear();
            for(int nn=0; nn<MFramework.Instance.BreakObject.m_pmSettedElementList.Count; ++nn)
            {
                ObjectElementData kData = (ObjectElementData)MFramework.Instance.BreakObject.m_pmSettedElementList[nn];
                if (kData.m_nChangeFlag == 3)
                    continue;

                string strText = "Element:" + kData.m_nElementNo + " Object:" + kData.m_nObjectNo
                + " Pos:" + kData.m_fRelativeX + " " + kData.m_fRelativeY + " " + kData.m_fRelativeZ;

                listObjectElement.Items.Add(strText);
            }
        }

        private void LoadMonsterBagControl()
        {
            string source =
                "Provider=SQLOLEDB;" +
                "server=" + m_strServer + ";" +
                "uid=" + m_strID + ";pwd=" + m_strPassword + ";" +
                "database=" + m_strDatabase;
            string select = "SELECT ParentBagNo,DefaultBagNo,Level01BagNo,Level02BagNo,Level03BagNo,Level04BagNo,Level05BagNo,Level06BagNo" +
                " FROM " + m_strBagControlTable;// WHERE MapNo=" + iMapNo;

            OleDbConnection conn = new OleDbConnection(source);
            OleDbDataReader aReader = null;
            if (conn == null)
            {
                MessageBox.Show("Connection is null");
                return;
            }

            try
            {
                conn.Open();
                OleDbCommand cmd = new OleDbCommand(select, conn);
                aReader = cmd.ExecuteReader();
                if (aReader.FieldCount > 0)
                {
                    while (aReader.Read())
                    {
                        int iParentBagNo = aReader.GetOrdinal("ParentBagNo");
                        int iDefaultBagNo = aReader.GetOrdinal("DefaultBagNo");
                        int iEasyBagNo = aReader.GetOrdinal("Level01BagNo");
                        int iNormalBagNo = aReader.GetOrdinal("Level02BagNo");
                        int iHardBagNo = aReader.GetOrdinal("Level03BagNo");
                        int iUltraBagNo = aReader.GetOrdinal("Level04BagNo");
                        int iLevel05Bag = aReader.GetOrdinal("Level05BagNo");
                        int iLevel06Bag = aReader.GetOrdinal("Level06BagNo");

                        MonsterBagControl kData = new MonsterBagControl();

                        kData.Initilaize(
                            aReader.GetInt32(iParentBagNo),
                            aReader.GetInt32(iDefaultBagNo),
                            aReader.GetInt32(iEasyBagNo),
                            aReader.GetInt32(iNormalBagNo),
                            aReader.GetInt32(iHardBagNo),
                            aReader.GetInt32(iUltraBagNo),
                            aReader.GetInt32(iLevel05Bag),
                            aReader.GetInt32(iLevel06Bag)
                        );

                        m_pmMonsterBagControlList.Add(kData);
                    }
                }
            }
            catch (System.Exception e)
            {
                labelDBConnectState.Text = m_strServer + " Disconnected.";
                MessageBox.Show(e.ToString());
                return;
            }
            finally
            {
                try
                {
                    if (aReader != null)
                        aReader.Close();
                    conn.Close();
                }
                catch (System.Exception e)
                {
                    MessageBox.Show(e.ToString());
                }
            }

            labelDBConnectState.Text = m_strServer + " Connected.";
        }

        private void LoadMapMonsterRegenFromDB(int iMapNo)
        {
            string source =
                "Provider=SQLOLEDB;" +
                "server=" + m_strServer + ";" +
                "uid=" + m_strID + ";pwd=" + m_strPassword + ";" +
                "database=" + m_strDatabase;
			string select = "SELECT PointGuid,Memo,MapNo,ParentBagType,ParentBagNo,PointGroup,RegenPeriod,PosX,PosY,PosZ,MoveRange,Direction " +
                            " FROM " + m_strRegenTable;// WHERE MapNo=" + iMapNo;

            OleDbConnection conn = new OleDbConnection(source);
            OleDbDataReader aReader = null;
            if (conn == null)
            {
                MessageBox.Show("Connection is null");
                return;
            }

            try
            {
                conn.Open();
                OleDbCommand cmd = new OleDbCommand(select, conn);
                aReader = cmd.ExecuteReader();
                if (aReader.FieldCount > 0)
                {
                    while (aReader.Read())
                    {
                        int iMapNoOrd = aReader.GetOrdinal("MapNo");
                        int iMapCheckNo = aReader.GetInt32(iMapNoOrd);
                        int iPointGuid = aReader.GetOrdinal("PointGuid");
                        int iParentBagTypeOrd = aReader.GetOrdinal("ParentBagType");// 1 몬스터 2 파괴오브젝트
                        byte byParentBagType = aReader.GetByte(iParentBagTypeOrd);
                        if (iMapCheckNo == iMapNo && byParentBagType == 2)
                        {
                            // 로드
                            int iMemoOrd = aReader.GetOrdinal("Memo");
                            int iMonParentBagNoOrd = aReader.GetOrdinal("ParentBagNo");
                            int iRegenPeriodOrd = aReader.GetOrdinal("RegenPeriod");
                            int iPointGroupOrd = aReader.GetOrdinal("PointGroup");
                            int iPosXOrd = aReader.GetOrdinal("PosX");
                            int iPosYOrd = aReader.GetOrdinal("PosY");
                            int iPosZOrd = aReader.GetOrdinal("PosZ");
							int iRotAxZOrd = aReader.GetOrdinal("Direction");
                            int iMoveRange = aReader.GetOrdinal("MoveRange");

                            string strMemo = "";
                            if (!aReader.IsDBNull(iMemoOrd))
                            {
                                strMemo = aReader.GetString(iMemoOrd);
                            }

                            Guid kGuid = aReader.GetGuid(iPointGuid);
                            int nMonParentBagNo = aReader.GetInt32(iMonParentBagNoOrd);
                            int nPointGroup = aReader.GetInt32(iPointGroupOrd);
                            int nRegenPeriod = aReader.GetInt32(iRegenPeriodOrd);
                            float fPosX = (float)aReader.GetDouble(iPosXOrd);
                            float fPosY = (float)aReader.GetDouble(iPosYOrd);
                            float fPosZ = (float)aReader.GetDouble(iPosZOrd);
							int iRotAxZ = (int)aReader.GetByte(iRotAxZOrd);
                            int nMoveRange = aReader.GetInt32(iMoveRange);

                            string strGroupNum = GetGroupNumToBagControl(nMonParentBagNo);

                            MFramework.Instance.BreakObject.AddSettedBreakObjectData(
                                nMonParentBagNo.ToString(),
                                kGuid, strMemo, iMapCheckNo, nMonParentBagNo,
                                nPointGroup,
                                nRegenPeriod, fPosX, fPosY, fPosZ, iRotAxZ,
                                nMoveRange,
                                strGroupNum
                                );
                        }
                    }
                }
            }
            catch (System.Exception e)
            {
                labelDBConnectState.Text = m_strServer + " Disconnected.";
                MessageBox.Show(e.ToString());
                return;
            }
            finally
            {
                try
                {
                    if (aReader != null)
                        aReader.Close();
                    conn.Close();
                }
                catch (System.Exception e)
                {
                    MessageBox.Show(e.ToString());
                }
            }

            labelDBConnectState.Text = m_strServer + " Connected.";
        }

        private void LoadObjectBagFromDB()
        {
            string source =
                "Provider=SQLOLEDB;" + "server=" + m_strServer +
                ";uid=" + m_strID + ";pwd=" + m_strPassword +
                ";database=" + m_strDatabase;
            string select = "SELECT BagNo,ObjElement1,ObjElement2,ObjElement3,ObjElement4,ObjElement5,ObjElement6,ObjElement7,ObjElement8,ObjElement9,ObjElement10" +
                " FROM " + m_strObjectBagTalbe;

            OleDbConnection conn = new OleDbConnection(source);
            OleDbDataReader aReader = null;

            if (conn == null)
            {
                MessageBox.Show("Connection is null");
                return;
            }

            try
            {
                conn.Open();
                OleDbCommand cmd = new OleDbCommand(select, conn);
                aReader = cmd.ExecuteReader();
                if(aReader.FieldCount > 0)
                {
                    while(aReader.Read())
                    {
                        int iBagNoOrd = aReader.GetOrdinal("BagNo");
                        int iObjElement1Ord = aReader.GetOrdinal("ObjElement1");
                        int iObjElement2Ord = aReader.GetOrdinal("ObjElement2");
                        int iObjElement3Ord = aReader.GetOrdinal("ObjElement3");
                        int iObjElement4Ord = aReader.GetOrdinal("ObjElement4");
                        int iObjElement5Ord = aReader.GetOrdinal("ObjElement5");
                        int iObjElement6Ord = aReader.GetOrdinal("ObjElement6");
                        int iObjElement7Ord = aReader.GetOrdinal("ObjElement7");
                        int iObjElement8Ord = aReader.GetOrdinal("ObjElement8");
                        int iObjElement9Ord = aReader.GetOrdinal("ObjElement9");
                        int iObjElement10Ord = aReader.GetOrdinal("ObjElement10");

                        //ObjectBag kData = new ObjectBag();

                        //kData.Initialize(
                        //    aReader.GetInt32(iBagNoOrd),
                        //    aReader.GetInt32(iObjElement1Ord),
                        //    aReader.GetInt32(iObjElement2Ord),
                        //    aReader.GetInt32(iObjElement3Ord),
                        //    aReader.GetInt32(iObjElement4Ord),
                        //    aReader.GetInt32(iObjElement5Ord),
                        //    aReader.GetInt32(iObjElement6Ord),
                        //    aReader.GetInt32(iObjElement7Ord),
                        //    aReader.GetInt32(iObjElement8Ord),
                        //    aReader.GetInt32(iObjElement9Ord),
                        //    aReader.GetInt32(iObjElement10Ord)
                        //    );

                        //m_pmObjectBagList.Add(kData);


                        int nBagNo = aReader.GetInt32(iBagNoOrd);
                        int nObjElement1 = aReader.GetInt32(iObjElement1Ord);
                        int nObjElement2 = aReader.GetInt32(iObjElement2Ord);
                        int nObjElement3 = aReader.GetInt32(iObjElement3Ord);
                        int nObjElement4 = aReader.GetInt32(iObjElement4Ord);
                        int nObjElement5 = aReader.GetInt32(iObjElement5Ord);
                        int nObjElement6 = aReader.GetInt32(iObjElement6Ord);
                        int nObjElement7 = aReader.GetInt32(iObjElement7Ord);
                        int nObjElement8 = aReader.GetInt32(iObjElement8Ord);
                        int nObjElement9 = aReader.GetInt32(iObjElement9Ord);
                        int nObjElement10 = aReader.GetInt32(iObjElement10Ord);


                        MFramework.Instance.BreakObject.AddAllBagList(nBagNo, nObjElement1
                            , nObjElement2, nObjElement3, nObjElement4, nObjElement5
                            , nObjElement6, nObjElement7, nObjElement8, nObjElement9
                            , nObjElement10);
                    }
                }
            }
            catch (System.Exception e)
            {
            	labelDBConnectState.Text = m_strServer + " Disconnected.";
                MessageBox.Show(e.ToString());
            }
            finally
            {
                try
                {
                    if(aReader != null)
                        aReader.Close();
                    conn.Close();
                }
                catch (System.Exception e)
                {
                	MessageBox.Show(e.ToString());
                }
            }

            labelDBConnectState.Text = m_strServer + " Connected.";
        }

        private void LoadObjectElementFromDB()
        {
            string source =
                "Provider=SQLOLEDB;" +
                "server=" + m_strServer + ";" +
                "uid=" + m_strID + ";pwd=" + m_strPassword + ";" +
                "database=" + m_strDatabase;
            string select = "SELECT ElementNo,ObjectNo,RelativeX,RelativeY,RelativeZ" +
                " FROM " + m_strObjectElementTable;

            OleDbConnection conn = new OleDbConnection(source);
            OleDbDataReader aReader = null;
            if (conn == null)
            {
                MessageBox.Show("Connection is null");
                return;
            }

            try
            {
                conn.Open();
                OleDbCommand cmd = new OleDbCommand(select, conn);
                aReader = cmd.ExecuteReader();
                if(aReader.FieldCount > 0)
                {
                    while(aReader.Read())
                    {
                        int nElementNoOrd = aReader.GetOrdinal("ElementNo");
                        int nObjectNoOrd = aReader.GetOrdinal("ObjectNo");
                        int nRelativeXOrd = aReader.GetOrdinal("RelativeX");
                        int nRelativeYOrd = aReader.GetOrdinal("RelativeY");
                        int nRelativeZOrd = aReader.GetOrdinal("RelativeZ");


                        int nElementNo = aReader.GetInt32(nElementNoOrd);
                        int nObjectNo = aReader.GetInt32(nObjectNoOrd);
                        float fRelativeX = (float)aReader.GetFloat(nRelativeXOrd);
                        float fRelativeY = (float)aReader.GetFloat(nRelativeYOrd);
                        float fRelativeZ = (float)aReader.GetFloat(nRelativeZOrd);


                        string strPath = GetObjectFilePath(nObjectNo);
                        if(strPath != null && strPath.Length > 0)
                        {
                            MFramework.Instance.BreakObject.AddSettedElementList(nElementNo, nObjectNo,
                            fRelativeX, fRelativeY, fRelativeZ, strPath);
                        }
                        else
                        {
                            string strErr;
                            strErr = "Please Check ObjectNo: " + nObjectNo.ToString() + " in TB_DefObjectElement";
                            MessageBox.Show(strErr.ToString());
                        }
                    }
                }
            }
            catch (System.Exception e)
            {
                labelDBConnectState.Text = m_strServer + " Disconnected.";
                MessageBox.Show(e.ToString());
                return;
            }
            finally
            {
                try
                {
                    if (aReader != null)
                        aReader.Close();
                    conn.Close();
                }
                catch (System.Exception e)
                {
                    MessageBox.Show(e.ToString());
                }
            }

            labelDBConnectState.Text = m_strServer + " Connected.";
        }

        private void SaveMapMonsterRegenToDB(int nMapNo)
        {
            string source =
                "server=" + m_strServer + ";" +
                "uid=" + m_strID + ";pwd=" + m_strPassword + ";" +
                "database=" + m_strDatabase;

            SqlConnection conn = new SqlConnection(source);
            int iRowEffected;

            try
            {
                conn.Open();
                SqlCommand command;

                int nCount = MFramework.Instance.BreakObject.m_pmSettedBreakObjectList.Count;
                for(int nn=0; nn<nCount; ++nn)
                {
                    SettedObjectData kData = (SettedObjectData)MFramework.Instance.BreakObject.m_pmSettedBreakObjectList[nn];
                    if(kData.m_pkEntity != null)
                    {
                        MPoint3 kPoint;
                        kPoint = MFramework.Instance.BreakObject.GetBreakObjectPos(kData.m_pkEntity);
                        kData.m_fPosX = kPoint.X;
                        kData.m_fPosY = kPoint.Y;
                        kData.m_fPosZ = kPoint.Z;
						kData.m_iRotAxZ = MFramework.Instance.BreakObject.GetBreakObjectRot(kData.m_pkEntity);
                    }
                    else
                    {
                        MessageBox.Show("Entity is null");
                    }

                    //bool bDelete = false;
                    string strCmdText = "";
                    if (kData.m_nChangeFlag == 0)// DCV_NORMAL
                    {
                        continue;
                    }
                    //else if (kData.m_nChangeFlag == 1)// DCV_INSERT
                    //{
                    //    strCmdText =
                    //        "INSERT INTO " + m_strRegenTable +
                    //        "( PointGuid,  Memo,  MapNo,  ParentBagType,  ParentBagNo,  PointGroup,  RegenPeriod,  PosX,  PosY,  PosZ,  MoveRange) VALUES " +
                    //        "(@PointGuid, @Memo, @MapNo, @ParentBagType, @ParentBagNo, @PointGroup, @RegenPeriod, @PosX, @PosY, @PosZ, @MoveRange)";
                    //}
                    //else if (kData.m_nChangeFlag == 2)// DCV_UPDATE
                    //{
                    //    strCmdText =
                    //        "UPDATE " + m_strRegenTable + " SET PointGuid = @PointGuid, " +
                    //        "Memo = @Memo, " +
                    //        "MapNo = @MapNo, " +
                    //        "ParentBagType = @ParentBagType, " +
                    //        "ParentBagNo = @ParentBagNo, " +
                    //        "PointGroup = @PointGroup, " +
                    //        "RegenPeriod = @RegenPeriod, " +
                    //        "PosX = @PosX, " +
                    //        "PosY = @PosY, " +
                    //        "PosZ = @PosZ, " +
                    //        "MoveRange = @MoveRange, " +
                    //        "WHERE PointGuid='" + kData.m_kGuid + "'";
                    //}
                    //else if (kData.m_nChangeFlag == 3)// DCV_DELETE
                    //{
                    //    strCmdText =
                    //        "DELETE FROM " + m_strRegenTable + " " +
                    //        "WHERE PointGuid='" + kData.m_kGuid + "'";
                    //    bDelete = true;
                    //}

                    //command = new SqlCommand(strCmdText, conn);

                    //if (!bDelete)
                    //{
                    //    command.Parameters.Add("@UpdateType", SqlDbType.Int);
                    //    command.Parameters.Add("@PointGuid", SqlDbType.UniqueIdentifier);
                    //    command.Parameters.Add("@Memo", SqlDbType.NVarChar);
                    //    command.Parameters.Add("@MapNo", SqlDbType.Int);
                    //    command.Parameters.Add("@ParentBagType", SqlDbType.TinyInt);
                    //    command.Parameters.Add("@ParentBagNo", SqlDbType.Int);
                    //    command.Parameters.Add("@PointGroup", SqlDbType.Int);
                    //    command.Parameters.Add("@RegenPeriod", SqlDbType.Int);
                    //    command.Parameters.Add("@PosX", SqlDbType.Float);
                    //    command.Parameters.Add("@PosY", SqlDbType.Float);
                    //    command.Parameters.Add("@PosZ", SqlDbType.Float);
                    //    command.Parameters.Add("@MoveRange", SqlDbType.Int);
                    //    command.Parameters["@UpdateType"].Value = kData.m_nChangeFlag;
                    //    command.Parameters["@PointGuid"].Value = kData.m_kGuid;
                    //    command.Parameters["@Memo"].Value = kData.m_strMemo;
                    //    command.Parameters["@MapNo"].Value = kData.m_iMapNo;
                    //    command.Parameters["@ParentBagType"].Value = 2;
                    //    command.Parameters["@ParentBagNo"].Value = kData.m_iMonParentBagNo;
                    //    command.Parameters["@PointGroup"].Value = kData.m_iPointGroup;
                    //    command.Parameters["@RegenPeriod"].Value = kData.m_iRegenPeriod;
                    //    command.Parameters["@PosX"].Value = kData.m_fPosX;
                    //    command.Parameters["@PosY"].Value = kData.m_fPosY;
                    //    command.Parameters["@PosZ"].Value = kData.m_fPosZ;
                    //    command.Parameters["@MoveRange"].Value = kData.m_iMoveRange;
                    //}
                    //else
                    //{
                    //    command.Parameters.Add("@UpdateType", SqlDbType.Int);
                    //    command.Parameters.Add("@PointGuid", SqlDbType.UniqueIdentifier);
                    //    command.Parameters["@UpdateType"].Value = kData.m_nChangeFlag;
                    //    command.Parameters["@PointGuid"].Value = kData.m_kGuid;
                    //}

                    if (kData.m_nChangeFlag == 1 || kData.m_nChangeFlag == 2)
                    {
                        strCmdText =
                            "EXEC [dbo].[UP_MapTool_UpdateMonsterRegen] '" +
                            m_strRegenTable + "', " +
                            kData.m_nChangeFlag + ", '" +
                            kData.m_kGuid + "', '" +
                            kData.m_strMemo + "', " +
                            kData.m_iMapNo + ", " +
                            2 + ", " +
                            kData.m_iMonParentBagNo + ", " +
                            kData.m_iPointGroup + ", " +
                            kData.m_iRegenPeriod + ", " +
                            kData.m_fPosX + ", " +
                            kData.m_fPosY + ", " +
                            kData.m_fPosZ + ", " +
							kData.m_iMoveRange + ", " +
							kData.m_iRotAxZ;
                    }
                    else
                    {
                        strCmdText =
                            "EXEC [dbo].[UP_MapTool_UpdateMonsterRegen] '" +
                            m_strRegenTable + "', " +
                            kData.m_nChangeFlag + ", '" +
                            kData.m_kGuid + "'";
                    }

                    command = new SqlCommand(strCmdText, conn);
                    iRowEffected = command.ExecuteNonQuery();

                    if (iRowEffected == 0)
                    {
                        throw new System.Exception("effected row was 0");
                    }
                }
            }
            catch (System.Exception e)
            {
                MessageBox.Show(e.ToString());
                return;
            }
            finally
            {
                try
                {
                    conn.Close();
                }
                catch (System.Exception e)
                {
                    MessageBox.Show(e.ToString());
                }
            }

            MFramework.Instance.BreakObject.SettedObjectFlagSettingAfterDBUpdate();
        }

        private void SaveMapMonsterRegenToDB2(int iMapNo)
        {
            //string source =
            //    "server=" + m_strServer + ";" +
            //    "uid=" + m_strID + ";pwd=" + m_strPassword + ";" +
            //    "database=" + m_strDatabase;

            //SqlConnection conn = new SqlConnection(source);
            
            //int iRowEffected;

            //try
            //{
            //    conn.Open();
            //    SqlCommand command;

            //    for(int i=0; i<m_pmSettedMonsterList.Count; ++i)
            //    {
            //        MonsterData kData = (MonsterData)m_pmSettedMonsterList[i];
            //        if (kData.m_pkEntity != null)
            //        {
            //            // 최신 좌표 갱신.
            //            MPoint3 kPoint;
            //            kPoint = MFramework.Instance.BreakObject.GetBreakObjectPos(kData.m_pkEntity);
            //            kData.m_fPosX = kPoint.X;
            //            kData.m_fPosY = kPoint.Y;
            //            kData.m_fPosZ = kPoint.Z;
            //        }
            //        else
            //        {
            //            MessageBox.Show("Entity is null");
            //        }

            //        // Delete Check
            //        bool bIsDeleted = false;
            //        bool bDeleteNotCommit = false;
            //        int iCount = MFramework.Instance.BreakObject.m_pmDeletedBreakObjectList.Count;
            //        for (int j = 0; j < iCount; j++)
            //        {
            //            Guid kGuid = MFramework.Instance.BreakObject.GetDeletedBreakObjectGuidData(j);
            //            if (kData.m_kGuid == kGuid)
            //            {
            //                bIsDeleted = true;
            //                MFramework.Instance.BreakObject.m_pmDeletedBreakObjectList.Remove(
            //                    MFramework.Instance.BreakObject.m_pmDeletedBreakObjectList[j]
            //                    );

            //                bool bIsServer = false;
            //                for (int k = 0; k < m_pmMonsterGuidListFromServer.Count; k++)
            //                {
            //                    if (m_pmMonsterGuidListFromServer[k].Equals(kData.m_kGuid))
            //                    {
            //                        m_pmMonsterGuidListFromServer.RemoveAt(k);
            //                        bIsServer = true;
            //                        break;
            //                    }
            //                }

            //                MFramework.Instance.BreakObject.DeleteToDeletedBreakObjectGuidData(kGuid);

            //                if (!bIsServer)
            //                {
            //                    bDeleteNotCommit = true;
            //                }

            //                break;
            //            }
            //        }
            //        if (bDeleteNotCommit)
            //        {
            //            m_pmSettedMonsterList.Remove(kData);
            //            i -= 1;
            //            continue;
            //        }

            //        // Delete Check 를 통과했으면 Update인지 체크.
            //        bool bIsUpdate = false;
            //        if (!bIsDeleted)
            //        {
            //            for (int j = 0; j < m_pmMonsterGuidListFromServer.Count; j++)
            //            {
            //                if (m_pmMonsterGuidListFromServer[j].Equals(kData.m_kGuid))
            //                {
            //                    j = m_pmMonsterGuidListFromServer.Count;
            //                    bIsUpdate = true;
            //                }
            //            }
            //        }

            //        if (bIsDeleted) // Deleted
            //        {
            //            command = new SqlCommand(
            //                "DELETE FROM " + m_strRegenTable + " " +
            //                "WHERE PointGuid='" + kData.m_kGuid + "'",
            //                conn);
            //        }
            //        else if (bIsUpdate) // Updated
            //        {
            //            command = new SqlCommand(
            //                "UPDATE " + m_strRegenTable + " SET PointGuid = @PointGuid, " +
            //                "Memo = @Memo, " +
            //                "MapNo = @MapNo, " +
            //                "ParentBagType = @ParentBagType, " +
            //                "ParentBagNo = @ParentBagNo, " +
            //                "PointGroup = @PointGroup, " +
            //                "RegenPeriod = @RegenPeriod, " +
            //                "PosX = @PosX, " +
            //                "PosY = @PosY, " +
            //                "PosZ = @PosZ, " +
            //                "MoveRange = @MoveRange " +
            //                "WHERE PointGuid='" + kData.m_kGuid + "'",
            //                conn);
            //        }
            //        else // Insert
            //        {
            //            command = new SqlCommand(
            //                "INSERT INTO " + m_strRegenTable +
            //                "( PointGuid,  Memo,  MapNo,  ParentBagType,  ParentBagNo,  PointGroup,  RegenPeriod,  PosX,  PosY,  PosZ,  MoveRange,  ItemBag01,  ItemBag02,  ItemBag03 ) VALUES " +
            //                "(@PointGuid, @Memo, @MapNo, @ParentBagType, @ParentBagNo, @PointGroup, @RegenPeriod, @PosX, @PosY, @PosZ, @MoveRange, @ItemBag01, @ItemBag02, @ItemBag03)", conn);

            //            m_pmMonsterGuidListFromServer.Add(kData.m_kGuid);
            //        }

            //        if (!bIsDeleted)
            //        {
            //            command.Parameters.Add("@PointGuid", SqlDbType.UniqueIdentifier);
            //            command.Parameters.Add("@Memo", SqlDbType.NVarChar);
            //            command.Parameters.Add("@MapNo", SqlDbType.Int);
            //            command.Parameters.Add("@ParentBagType", SqlDbType.TinyInt);
            //            command.Parameters.Add("@ParentBagNo", SqlDbType.Int);
            //            command.Parameters.Add("@PointGroup", SqlDbType.Int);
            //            command.Parameters.Add("@RegenPeriod", SqlDbType.Int);
            //            command.Parameters.Add("@PosX", SqlDbType.Float);
            //            command.Parameters.Add("@PosY", SqlDbType.Float);
            //            command.Parameters.Add("@PosZ", SqlDbType.Float);
            //            command.Parameters.Add("@MoveRange", SqlDbType.Int);
            //            command.Parameters.Add("@ItemBag01", SqlDbType.Int);
            //            command.Parameters.Add("@ItemBag02", SqlDbType.Int);
            //            command.Parameters.Add("@ItemBag03", SqlDbType.Int);
            //            command.Parameters["@PointGuid"].Value = kData.m_kGuid;
            //            command.Parameters["@Memo"].Value = kData.m_strMemo;
            //            command.Parameters["@MapNo"].Value = kData.m_iMapNo;
            //            command.Parameters["@ParentBagType"].Value = 2;
            //            command.Parameters["@ParentBagNo"].Value = kData.m_iMonParentBagNo;
            //            command.Parameters["@PointGroup"].Value = kData.m_iPointGroup;
            //            command.Parameters["@RegenPeriod"].Value = kData.m_iRegenPeriod;
            //            command.Parameters["@PosX"].Value = kData.m_fPosX;
            //            command.Parameters["@PosY"].Value = kData.m_fPosY;
            //            command.Parameters["@PosZ"].Value = kData.m_fPosZ;
            //            command.Parameters["@MoveRange"].Value = kData.m_iMoveRange;
            //            command.Parameters["@ItemBag01"].Value = 0;
            //            command.Parameters["@ItemBag02"].Value = 0;
            //            command.Parameters["@ItemBag03"].Value = 0;
            //        }
            //        else
            //        {
            //        }

            //        iRowEffected = command.ExecuteNonQuery();

            //        if (iRowEffected == 0)
            //        {
            //            throw new System.Exception("effected row was 0");
            //        }

            //        if (bIsDeleted) // Deleted
            //        {
            //            m_pmSettedMonsterList.Remove(kData);
            //            i -= 1;
            //        }
            //    }                
            //}
            //catch (System.Exception e)
            //{
            //    MessageBox.Show(e.ToString());
            //    return;
            //}
            //finally
            //{
            //    try
            //    {
            //        conn.Close();
            //    }
            //    catch (System.Exception e)
            //    {
            //        MessageBox.Show(e.ToString());
            //    }
            //}
        }

        private bool SaveObjectBagToDB()
        {
            string source =
                "server=" + m_strServer + ";" +
                "uid=" + m_strID + ";pwd=" + m_strPassword + ";" +
                "database=" + m_strDatabase;

            SqlConnection conn = new SqlConnection(source);

            try
            {
                conn.Open();
                SqlCommand command;
                int iRowEffected;
                int nCount = MFramework.Instance.BreakObject.m_pmAllBagList.Count;
                for (int nn = 0; nn < nCount; ++nn )
                {
                    bool bDelete = false;
                    string strCmdText = "";
                    ObjectBagData kData = (ObjectBagData)MFramework.Instance.BreakObject.m_pmAllBagList[nn];
                    if(kData.m_nChangeFlag == 0)// DCV_NORMAL
                    {
                        continue;
                    }
                    else if(kData.m_nChangeFlag == 1)// DCV_INSERT
                    {
                        strCmdText =
                            "INSERT INTO " + m_strObjectBagTalbe +
                            "(BagNo,ObjElement1,ObjElement2,ObjElement3,ObjElement4,ObjElement5,ObjElement6,ObjElement7,ObjElement8,ObjElement9,ObjElement10) VALUES " +
                            "(@BagNo,@ObjElement1,@ObjElement2,@ObjElement3,@ObjElement4,@ObjElement5,@ObjElement6,@ObjElement7,@ObjElement8,@ObjElement9,@ObjElement10)";
                    }
                    else if(kData.m_nChangeFlag == 2)// DCV_UPDATE
                    {
                        strCmdText =
                            "UPDATE " + m_strObjectBagTalbe + " SET BagNo = @BagNo, " +
                            "ObjElement1 = @ObjElement1, " +
                            "ObjElement2 = @ObjElement2, " +
                            "ObjElement3 = @ObjElement3, " +
                            "ObjElement4 = @ObjElement4, " +
                            "ObjElement5 = @ObjElement5, " +
                            "ObjElement6 = @ObjElement6, " +
                            "ObjElement7 = @ObjElement7, " +
                            "ObjElement8 = @ObjElement8, " +
                            "ObjElement9 = @ObjElement9, " +
                            "ObjElement10 = @ObjElement10 " +
                            "WHERE BagNo ='" + kData.m_nBagNo + "'";
                    }
                    else if(kData.m_nChangeFlag == 3)// DCV_DELETE
                    {
                        strCmdText =
                            "DELETE FROM " + m_strObjectBagTalbe + " " +
                            "WHERE BagNo='" + kData.m_nBagNo + "'";
                        bDelete = true;
                    }

                    command = new SqlCommand(strCmdText, conn);

                    if(!bDelete)
                    {
                        command.Parameters.Add("@BagNo", SqlDbType.Int);
                        command.Parameters.Add("@ObjElement1", SqlDbType.Int);
                        command.Parameters.Add("@ObjElement2", SqlDbType.Int);
                        command.Parameters.Add("@ObjElement3", SqlDbType.Int);
                        command.Parameters.Add("@ObjElement4", SqlDbType.Int);
                        command.Parameters.Add("@ObjElement5", SqlDbType.Int);
                        command.Parameters.Add("@ObjElement6", SqlDbType.Int);
                        command.Parameters.Add("@ObjElement7", SqlDbType.Int);
                        command.Parameters.Add("@ObjElement8", SqlDbType.Int);
                        command.Parameters.Add("@ObjElement9", SqlDbType.Int);
                        command.Parameters.Add("@ObjElement10", SqlDbType.Int);
                        command.Parameters["@BagNo"].Value = kData.m_nBagNo;
                        command.Parameters["@ObjElement1"].Value = kData.m_nObjElement1;
                        command.Parameters["@ObjElement2"].Value = kData.m_nObjElement2;
                        command.Parameters["@ObjElement3"].Value = kData.m_nObjElement3;
                        command.Parameters["@ObjElement4"].Value = kData.m_nObjElement4;
                        command.Parameters["@ObjElement5"].Value = kData.m_nObjElement5;
                        command.Parameters["@ObjElement6"].Value = kData.m_nObjElement6;
                        command.Parameters["@ObjElement7"].Value = kData.m_nObjElement7;
                        command.Parameters["@ObjElement8"].Value = kData.m_nObjElement8;
                        command.Parameters["@ObjElement9"].Value = kData.m_nObjElement9;
                        command.Parameters["@ObjElement10"].Value = kData.m_nObjElement10;
                    }

                    iRowEffected = command.ExecuteNonQuery();

                    if(iRowEffected == 0)
                    {
                        throw new System.Exception("effected row was 0");
                    }
                }

                MFramework.Instance.BreakObject.ObjectBagFlagSettingAfterDBUpdate();
            }
            catch (System.Exception e)
            {
                MessageBox.Show(e.ToString());
                return false;
            }
            finally
            {
                try
                {
                    conn.Close();
                }
                catch (System.Exception e)
                {
                    MessageBox.Show(e.ToString());
                }
            }

            return true;
        }

        private bool SaveObjectElementToDB()
        {
            string source =
                "server=" + m_strServer + ";" +
                "uid=" + m_strID + ";pwd=" + m_strPassword + ";" +
                "database=" + m_strDatabase;

            SqlConnection conn = new SqlConnection(source);

            try
            {
                conn.Open();
                SqlCommand command;
                int iRowEffected;
                int nCount = MFramework.Instance.BreakObject.m_pmSettedElementList.Count;
                for (int nn = 0; nn < nCount; ++nn)
                {
                    bool bDelete = false;
                    string strCmdText = "";
                    ObjectElementData kData = (ObjectElementData)MFramework.Instance.BreakObject.m_pmSettedElementList[nn];
                    if (kData.m_nChangeFlag == 0)// DCV_NORMAL
                    {
                        continue;
                    }
                    else if(kData.m_nChangeFlag == 1)// DCV_INSERT
                    {
                        strCmdText =
                            "INSERT INTO " + m_strObjectElementTable +
                            "(ElementNo,ObjectNo,RelativeX,RelativeY,RelativeZ) VALUES " +
                            "(@ElementNo,@ObjectNo,@RelativeX,@RelativeY,@RelativeZ)";
                    }
                    else if (kData.m_nChangeFlag == 2)// DCV_UPDATE
                    {
                        strCmdText =
                            "UPDATE " + m_strObjectElementTable + " SET ElementNo = @ElementNo, " +
                            "ObjectNo = @ObjectNo, " +
                            "RelativeX = @RelativeX, " +
                            "RelativeY = @RelativeY, " +
                            "RelativeZ = @RelativeZ " +
                            "WHERE ElementNo='" + kData.m_nElementNo + "'";
                    }
                    else if (kData.m_nChangeFlag == 3)// DCV_DELETE
                    {
                        strCmdText =
                            "DELETE FROM " + m_strObjectElementTable + " " +
                            "WHERE ElementNo='" + kData.m_nElementNo + "'";
                        bDelete = true;
                    }

                    command = new SqlCommand(strCmdText, conn);
                    
                    if(!bDelete)
                    {
                        command.Parameters.Add("@ElementNo", SqlDbType.Int);
                        command.Parameters.Add("@ObjectNo", SqlDbType.Int);
                        command.Parameters.Add("@RelativeX", SqlDbType.Real);
                        command.Parameters.Add("@RelativeY", SqlDbType.Real);
                        command.Parameters.Add("@RelativeZ", SqlDbType.Real);
                        command.Parameters["@ElementNo"].Value = kData.m_nElementNo;
                        command.Parameters["@ObjectNo"].Value = kData.m_nObjectNo;
                        command.Parameters["@RelativeX"].Value = kData.m_fRelativeX;
                        command.Parameters["@RelativeY"].Value = kData.m_fRelativeY;
                        command.Parameters["@RelativeZ"].Value = kData.m_fRelativeZ;
                    }
                    
                    iRowEffected = command.ExecuteNonQuery();

                    if (iRowEffected == 0)
                    {
                        throw new System.Exception("effected row was 0");
                    }
                }
            }
            catch (System.Exception e)
            {
                MessageBox.Show(e.ToString());
                return false;
            }
            finally
            {
                try
                {
                    conn.Close();
                }
                catch (System.Exception e)
                {
                    MessageBox.Show(e.ToString());
                }
            }

            labelDBConnectState.Text = m_strServer + " Connected.";

            MFramework.Instance.BreakObject.ObjectElementFlagSettingAfterDBUpdate();
            return true;
        }

        public void SetGroupList()
        {
            //listGroup.Items.Clear();
            //int nCount = MFramework.Instance.BreakObject.m_pmGroupList.Count;
            //for (int nn = 0; nn < nCount; ++nn)
            //{
            //    string strName = MFramework.Instance.BreakObject.GetGroupDataGroupName(nn);
            //    listGroup.Items.Add(strName);
            //}
        }

        public void SetSettedBagList()
        {
            listGroup.Items.Clear();
            int nCount = MFramework.Instance.BreakObject.m_pmSettedBreakObjectList.Count;
            for (int nn = 0; nn < nCount; ++nn)
            {
                //int nNum = MFramework.Instance.BreakObject.GetSettedMonsterBagNo(nn);
                SettedObjectData kData = (SettedObjectData)MFramework.Instance.BreakObject.m_pmSettedBreakObjectList[nn];
                if (kData.m_nChangeFlag == 3)
                    continue;

                string strText = "BagNo : " + kData.m_iMonParentBagNo;
                listGroup.Items.Add(strText);
            }
        }

        public void SetXmlManager(ref XmlManager pXmlManager)
        {
            m_XmlManager = pXmlManager;
        }

        public string GetObjectFilePath(int nObjectNumber)
        {
            // xml 파싱을 해서 오브젝트의 파일 패스를 알아오자
            string strObjectNumber = "";// ClassNo.xml 에서 이것으로 찾는다
            string strObjectID = "";// ClassNo.xml 에서 찾은 결과로서 path.xml 에서 파일패스 찾는데 사용한다..
            string strObjectPath = "";// path.xml 에서 찾아서 파일 패스를 저장한다.
            string strFilePath = "";// 결과물(kfm 패스)
            strObjectNumber = nObjectNumber.ToString();
            // ClassNo.xml
            if (m_strXmlPath == null || m_strXmlPath.Length == 0)
            {
                MessageBox.Show("Set XmlPath");
                return null;
            }
            string strClassNo = m_strXmlPath + "\\" + "ClassNo.xml";
            m_XmlManager.XmlFileLoad(strClassNo);
            XmlDocument pDoc = m_XmlManager.GetDocument();
            XmlNodeList nodeList = pDoc.GetElementsByTagName("ITEM");
            for (int n2 = 0; n2 < nodeList.Count; ++n2)
            {
                XmlElement child = (XmlElement)nodeList.Item(n2);
                if (child == null) continue;
                string strID = child.GetAttribute("NO");
                if (strID.Equals(strObjectNumber))
                {
                    strObjectID = child.GetAttribute("ID");
                    break;
                }
            }
            // Path.xml
            string strPath = m_strXmlPath + "\\" + "path.xml";
            m_XmlManager.XmlFileLoad(strPath);
            pDoc = m_XmlManager.GetDocument();
            nodeList = pDoc.GetElementsByTagName("ACTOR");
            for (int n4 = 0; n4 < nodeList.Count; ++n4)
            {
                XmlElement child = (XmlElement)nodeList.Item(n4);
                if (child == null) continue;
                string strID = child.GetAttribute("ID");
                if (strID != null && strID.Equals(strObjectID))
                {
                    strObjectPath = child.InnerText;
                    break;
                }
            }

            // mon/파일이름
            if (strObjectPath == null || strObjectPath.Length == 0)
                return null;
                
            string strLast = m_strXmlPath + "\\" + strObjectPath;
            m_XmlManager.XmlFileLoad(strLast);
            pDoc = m_XmlManager.GetDocument();
            nodeList = pDoc.GetElementsByTagName("KFMPATH");
            XmlElement childMon = (XmlElement)nodeList.Item(0);
            if (childMon == null)
            {
                strFilePath = "";
            }
            else
            {
                strFilePath = childMon.InnerText;
            }

            return strFilePath;
        }

        private void btnAdd_Click(object sender, EventArgs e)
        {
            if (tbElementNo != null && tbObjectNumber != null && tbPosX != null && tbPosY != null && tbPosZ != null)
            {
                int nElementNo, nObjectNumber;
                float fPosX, fPosY, fPosZ;
                bool bCheck = int.TryParse(tbElementNo.Text, out nElementNo);
                if (!bCheck)
                {
                    MessageBox.Show("ElementNumber Failed");
                    return;
                }
                bCheck = int.TryParse(tbObjectNumber.Text, out nObjectNumber);
                if (!bCheck)
                {
                    MessageBox.Show("ObjectNumber Failed");
                    return;
                }
                bCheck = float.TryParse(tbPosX.Text, out fPosX);
                if (!bCheck)
                {
                    MessageBox.Show("PosX Failed");
                    return;
                }
                bCheck = float.TryParse(tbPosY.Text, out fPosY);
                if (!bCheck)
                {
                    MessageBox.Show("PosY Failed");
                    return;
                }
                bCheck = float.TryParse(tbPosZ.Text, out fPosZ);
                if (!bCheck)
                {
                    MessageBox.Show("PosZ Failed");
                    return;
                }

                string strFilePath = GetObjectFilePath(nObjectNumber);
                //////////////////////////////////////////////////////////////////////////
                //// xml 파싱을 해서 오브젝트의 파일 패스를 알아오자
                //string strObjectNumber = "";// ClassNo.xml 에서 이것으로 찾는다
                //string strObjectID = "";// ClassNo.xml 에서 찾은 결과로서 path.xml 에서 파일패스 찾는데 사용한다..
                //string strObjectPath = "";// path.xml 에서 찾아서 파일 패스를 저장한다.
                //string strFilePath = "";// 결과물(kfm 패스)
                //strObjectNumber = nObjectNumber.ToString();
                //// ClassNo.xml
                //if (m_strXmlPath == null || m_strXmlPath.Length == 0)
                //{
                //    MessageBox.Show("Set XmlPath");
                //    return;
                //}
                //string strClassNo = m_strXmlPath + "ClassNo.xml";
                //m_XmlManager.XmlFileLoad(strClassNo);
                //XmlDocument pDoc = m_XmlManager.GetDocument();
                //XmlNodeList nodeList = pDoc.GetElementsByTagName("ITEM");
                //for (int n2 = 0; n2 < nodeList.Count; ++n2)
                //{
                //    XmlElement child = (XmlElement)nodeList.Item(n2);
                //    if (child == null) continue;
                //    string strID = child.GetAttribute("NO");
                //    if (strID.Equals(strObjectNumber))
                //    {
                //        strObjectID = child.GetAttribute("ID");
                //        break;
                //    }
                //}
                //// Path.xml
                //string strPath = m_strXmlPath + "path.xml";
                //m_XmlManager.XmlFileLoad(strPath);
                //pDoc = m_XmlManager.GetDocument();
                //nodeList = pDoc.GetElementsByTagName("ACTOR");
                //for (int n4 = 0; n4 < nodeList.Count; ++n4)
                //{
                //    XmlElement child = (XmlElement)nodeList.Item(n4);
                //    if (child == null) continue;
                //    string strID = child.GetAttribute("ID");
                //    if (strID != null && strID.Equals(strObjectID))
                //    {
                //        strObjectPath = child.InnerText;
                //        break;
                //    }
                //}

                //// mon/파일이름
                //if (strObjectPath != null)
                //{
                //    string strLast = m_strXmlPath + strObjectPath;
                //    m_XmlManager.XmlFileLoad(strLast);
                //    pDoc = m_XmlManager.GetDocument();
                //    nodeList = pDoc.GetElementsByTagName("KFMPATH");
                //    XmlElement child = (XmlElement)nodeList.Item(0);
                //    if (child == null)
                //    {
                //        strFilePath = "";
                //    }
                //    else
                //    {
                //        strFilePath = child.InnerText;
                //    }
                //}
                //////////////////////////////////////////////////////////////////////////
                //if (MFramework.Instance.BreakObject.AddObjectData(nObjectNumber, fPosX, fPosY, fPosZ, strFilePath))
                //{
                //    string strText = "Num:" + nObjectNumber + " PosXYZ:" + fPosX + fPosY + fPosZ;
                //    listObject.Items.Add(strText);
                //}
                if( nElementNo == 0 ||
                    !MFramework.Instance.BreakObject.AddElementData(nElementNo, nObjectNumber, fPosX, fPosY, fPosZ, strFilePath))
                {
                    MessageBox.Show("ElementNo 를 확인해 주세요");
                    return;
                }
                
                string strText = "ENum:" + nElementNo + " ONum:" + nObjectNumber + " X:" + fPosX + " Y:" + fPosY + " Z:" + fPosZ;
                listObject.Items.Add(strText);
            }
        }

        private void btnModify_Click(object sender, EventArgs e)
        {
            DialogResult result = MessageBox.Show(
                "Element 내용을 변경하시겠습니까?", "내용 변경", MessageBoxButtons.OKCancel);

            if (result == DialogResult.OK)
            {
                if (tbElementNo != null &&  tbObjectNumber != null && tbPosX != null
                    && tbPosY != null && tbPosZ != null)
                {
                    int nObjectNumber, nElementNo;
                    float fPosX, fPosY, fPosZ;
                    bool bCheck = int.TryParse(tbElementNo.Text, out nElementNo);
                    if (!bCheck)
                    {
                        MessageBox.Show("ElementNumber Failed");
                        return;
                    }
                    bCheck = int.TryParse(tbObjectNumber.Text, out nObjectNumber);
                    if (!bCheck)
                    {
                        MessageBox.Show("ObjectNumber Failed");
                        return;
                    }
                    bCheck = float.TryParse(tbPosX.Text, out fPosX);
                    if (!bCheck)
                    {
                        MessageBox.Show("PosX Failed");
                        return;
                    }
                    bCheck = float.TryParse(tbPosY.Text, out fPosY);
                    if (!bCheck)
                    {
                        MessageBox.Show("PosY Failed");
                        return;
                    }
                    bCheck = float.TryParse(tbPosZ.Text, out fPosZ);
                    if (!bCheck)
                    {
                        MessageBox.Show("PosZ Failed");
                        return;
                    }

                    bool bModify = MFramework.Instance.BreakObject.ModifyElement(nElementNo, nObjectNumber, fPosX, fPosY, fPosZ);

                    if (!bModify)
                    {
                        MessageBox.Show("Not find Element");
                        return;
                    }

                    MFramework.Instance.BreakObject.RefreshScreenBreakObject();

                    SyncObjectElement();
                }

            }
            //if(tbObjectNumber != null && tbPosZ != null)
            //{
            //    int nObjectNumber, nElementNo;
            //    float fPosX, fPosY, fPosZ;
            //    bool bCheck = int.TryParse(tbElementNo.Text, out nElementNo);
            //    if (!bCheck)
            //    {
            //        MessageBox.Show("ElementNumber Failed");
            //        return;
            //    }
            //    bCheck = int.TryParse(tbObjectNumber.Text, out nObjectNumber);
            //    if (!bCheck)
            //        return;
            //    bCheck = float.TryParse(tbPosX.Text, out fPosX);
            //    if (!bCheck)
            //        return;

            //    bCheck = float.TryParse(tbPosY.Text, out fPosY);
            //    if (!bCheck)
            //        return;

            //    bCheck = float.TryParse(tbPosZ.Text, out fPosZ);
            //    if (!bCheck)
            //        return;

            //    if (tbGroupName == null || tbGroupName.TextLength == 0)
            //        return;

            //    int nIndex = listObject.SelectedIndex;

            //    if (nIndex < 0)
            //        return;

            //    if(MFramework.Instance.BreakObject.ModifyObjectData(nIndex, nObjectNumber, fPosX, fPosY, fPosZ))
            //    {
            //        string strText = "ENum:" + nElementNo + " ONum:" + nObjectNumber + " X:" + fPosX + " Y:" + fPosY + " Z:" + fPosZ;
            //        listObject.Items.RemoveAt(nIndex);
            //        listObject.Items.Insert(nIndex, strText);
            //    }
            //}
        }

        private void btnRemove_Click(object sender, EventArgs e)
        {
            int nIndex = listObject.SelectedIndex;

            if (nIndex < 0)
                return;

            if(MFramework.Instance.BreakObject.RemoveObjectData(nIndex))
            {
                listObject.Items.RemoveAt(nIndex);
            }
        }

        private void btnCreate_Click(object sender, EventArgs e)
        {
            if (tbGroupName == null)
                return;

            if (tbRegenTime == null)
                return;

            if (tbGroupNum == null)
                return;

            if (m_strXmlPath.Length == 0)
                return;

            //int nCount = MFramework.Instance.BreakObject.m_pMakingData.m_pmObjectData.Count;
            ////int[] nObjectNumber = new int[nCount];
            //string[] strObjectNumber = new string[nCount];// ClassNo.xml 에서 이것으로 찾는다
            //string[] strObjectID = new string[nCount];// ClassNo.xml 에서 찾은 결과로서 path.xml 에서 파일패스 찾는데 사용한다..
            //string[] strObjectPath = new string[nCount];// path.xml 에서 찾아서 파일 패스를 저장한다.
            //string[] strFilePath = new string[nCount];// 결과물(kfm 패스)
            //for (int nn = 0; nn < nCount; ++nn)
            //{
            //    int nObjectNumber = MFramework.Instance.BreakObject.GetObjectDataObjectNumber(nn);
            //    int.TryParse(strObjectNumber[nn], out nObjectNumber);
            //}
            //// ClassNo.xml
            //string strClassNo = m_strXmlPath + "ClassNo.xml";
            //m_XmlManager.XmlFileLoad(strClassNo);
            //XmlDocument pDoc = m_XmlManager.GetDocument();
            //XmlNodeList nodeList = pDoc.GetElementsByTagName("ITEM");
            //for (int n1 = 0; n1 < nCount; ++n1)
            //{
            //    for (int n2 = 0; n2 < nodeList.Count; ++n2)
            //    {
            //        XmlElement child = (XmlElement)nodeList.Item(n2);
            //        if (child == null) continue;
            //        string strID = child.GetAttribute("NO");
            //        if(strID.Equals(strObjectNumber[n1]))
            //        {
            //            strObjectID[n1] = child.GetAttribute("ID");
            //            break;
            //        }
            //    }
            //}

            //// Path.xml
            //string strPath = m_strXmlPath + "path.xml";
            //m_XmlManager.XmlFileLoad(strPath);
            //pDoc = m_XmlManager.GetDocument();
            //nodeList = pDoc.GetElementsByTagName("ACTOR");
            //for (int n3 = 0; n3 < nCount; ++n3 )
            //{
            //    for(int n4=0; n4<nodeList.Count; ++n4)
            //    {
            //        XmlElement child = (XmlElement)nodeList.Item(n4);
            //        if(child == null) continue;
            //        string strID = child.GetAttribute("ID");
            //        if(strID.Equals(strObjectID))
            //        {
            //            strObjectPath[n4] = child.GetAttribute("PATH");
            //            break;
            //        }
            //    }
            //}
            //// mon/파일이름
            //for(int mm=0; mm<nCount; ++mm)
            //{
            //    if(strObjectPath[mm] != null)
            //    {
            //        m_XmlManager.XmlFileLoad(strObjectPath[mm]);
            //        pDoc = m_XmlManager.GetDocument();
            //        XmlElement child = pDoc.GetElementById("KFMPATH");
            //        if (child == null)
            //        {
            //            strFilePath[mm] = "";
            //            continue;
            //        }
            //        strFilePath[mm] = child.Value;
            //    }
            //}

            MPalette pkPalette = MFramework.Instance.PaletteManager.GetPaletteByName("General");
            if (pkPalette == null)
            {
                MessageBox.Show("General 팔레트가 없습니다.");
                return;
            }
            MEntity pkTemplate = pkPalette.GetEntityByName("[General]Object.break_object");
            if (pkTemplate == null)
            {
                MessageBox.Show("[General]Object.break_object 가 없습니다.");
                return;
            }

            MFramework.Instance.PaletteManager.ActivePalette = pkPalette;
            if (MFramework.Instance.PaletteManager.ActivePalette.ActiveEntity !=
                pkTemplate)
            {
                MFramework.Instance.PaletteManager.ActivePalette.ActiveEntity = pkTemplate;
            }

            UICommand command = UICommandService.GetCommand("CreateInteractionMode");
            if (command != null)
            {
                command.DoClick(this, null);
            }

            // BreakObject
            //if (MFramework.Instance.BreakObject.AddGroupData())
            //{
            //    listGroup.Items.Add(tbGroupName.Text);
            //}
        }
		/*
        private void btnSave_Click(object sender, EventArgs e)
        {
            if (MFramework.Instance.BreakObject.SaveXml())
                MessageBox.Show("저장되었습니다.");
            else
                MessageBox.Show("저장이 실패하였습니다.");
        }

        private void btnLoad_Click(object sender, EventArgs e)
        {
            MFramework.Instance.BreakObject.LoadXml();
            SetGroupList();
        }
		*/
        private void btnGroupRemove_Click(object sender, EventArgs e)
        {
            int nIndex = listGroup.SelectedIndex;
            if (nIndex < 0)
                return;

            DialogResult result = MessageBox.Show(
                "그룹을 삭제하시겠습니까?", "삭제", MessageBoxButtons.OKCancel);

            if (result == DialogResult.OK)
            {
                //MFramework.Instance.BreakObject.RemoveGroupData(nIndex);
                //listGroup.Items.RemoveAt(nIndex);
                //SetGroupList();
                MFramework.Instance.BreakObject.RemoveSettedBreakObject(nIndex);
                SetSettedBagList();
                SyncObjectBag();
            }
        }

        private void tbGroupName_TextChanged(object sender, EventArgs e)
        {
            int nBagNo;
            Int32.TryParse(tbGroupName.Text, out nBagNo);
            MFramework.Instance.BreakObject.m_pMakingData.m_nBagNo = nBagNo;
        }

        private void listGroup_SelectedIndexChanged(object sender, EventArgs e)
        {
            MFramework.Instance.BreakObject.GroupListSelect(listGroup.SelectedIndex);

            tbGroupName.Text = MFramework.Instance.BreakObject.m_pMakingData.m_nBagNo.ToString();
            tbRegenTime.Text = MFramework.Instance.BreakObject.m_pMakingData.m_nRegenPeriod.ToString();
            tbGroupNum.Text = MFramework.Instance.BreakObject.m_pMakingData.m_nPointGroup.ToString();
            listObject.Items.Clear();

            int nCount = MFramework.Instance.BreakObject.m_pMakingData.m_pmElementList.Count;
            for(int nn=0; nn<nCount;++nn)
            {
                ObjectElementData kData = (ObjectElementData)MFramework.Instance.BreakObject.m_pMakingData.m_pmElementList[nn];
                //int nObjectNumber = MFramework.Instance.BreakObject.GetObjectDataObjectNumber(nn);
                //int nHeight = MFramework.Instance.BreakObject.GetObjectDataHeight(nn);
                //string strText = "Num:" + nObjectNumber + " Height:" + nHeight;
                string strText = "ENum:" + kData.m_nElementNo + " ONum:" + kData.m_nObjectNo +
                    " X:" + kData.m_fRelativeX + " Y:" + kData.m_fRelativeY + " Z:" + kData.m_fRelativeZ;
                listObject.Items.Add(strText);
            }
        }

        private void btnObjectListClear_Click(object sender, EventArgs e)
        {
            //MFramework.Instance.BreakObject.ClearObjectData();
            MFramework.Instance.BreakObject.m_pMakingData.m_pmElementList.Clear();
            listObject.Items.Clear();
        }

        private void btnXmlPath_Click(object sender, EventArgs e)
        {
            // 저장할 xml 파일을 지정(XML\BreakObject)
            // XML 폴더패스 저장
            // XML\ClassNo.xml 에서 몬스터No 에서 아이디를 알아온다
            // XML\path.xml 에서 아이디에서 파일패스를 알아온다.
            // 파일패스의 xml 을열어서 해당 몬스터의 kfm 파일 패스를 알아온다.
            // kfm 파일의 확장자를 nif 로 바꿔 파일을 읽어오면 완료(상대경로이니 잘 조절할것)
            //SaveFileDialog dlg = new SaveFileDialog();

            //string streamingDescriptions = "XML files (*.xml)|*.xml";
            //string streamingExtensions = "xml";

            //dlg.Filter = streamingDescriptions;
            //if (dlg.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            //{
            //    tbXmlPath.Text = dlg.FileName;
            //    int nLastIndex = tbXmlPath.Text.LastIndexOf("BreakObject");
            //    if(nLastIndex == -1)
            //    {
            //        MessageBox.Show("BreakObject 폴더가 없습니다.");
            //        return;
            //    }
            //    m_strXmlPath = tbXmlPath.Text.Substring(0, nLastIndex);
            //    MFramework.Instance.BreakObject.m_strXmlPath = m_strXmlPath.ToString();
            //    MFramework.Instance.BreakObject.m_strFileName = tbXmlPath.Text.ToString();
            //}

            FolderBrowserDialog dlg = new FolderBrowserDialog();

            if (dlg.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                m_strXmlPath = dlg.SelectedPath;
                MFramework.Instance.BreakObject.m_strXmlPath = m_strXmlPath.ToString();
                tbXmlPath.Text = m_strXmlPath;
            }
        }

        #region WinForms event handlers
		[UICommandHandler("Idle")]
        private void Application_Idle(object sender, EventArgs e)
        {
            if(MFramework.Instance.BreakObject.AddGroup)
            {
                //SetGroupList();
                SetSettedBagList();
                SyncObjectBag();
                SyncObjectElement();
                MFramework.Instance.BreakObject.AddGroup = false;
            }

            if(MFramework.Instance.BreakObject.DeleteGroup)
            {
                //SetGroupList();
                SetSettedBagList();
                SyncObjectBag();
                SyncObjectElement();
                MFramework.Instance.BreakObject.DeleteGroup = false;
            }
        }
        #endregion

        private void btnDBSetting_Click(object sender, EventArgs e)
        {
            if (tbMapNumber == null || tbMapNumber.Text.Equals("0")) 
            {
                MessageBox.Show("맵번호를 입력하세요");
                return;
            }

            int nMapNumber;
            bool bRtn = Int32.TryParse(tbMapNumber.Text, out nMapNumber);

            MonsterPanelSetting frm = new MonsterPanelSetting();
            frm.m_strServer = m_strServer;
            frm.m_strID = m_strID;
            frm.m_strPassword = m_strPassword;
            frm.m_strDatabase = m_strDatabase;
            frm.m_strMonsterTable = "";
            frm.m_strRegenTable = m_strRegenTable;
            frm.m_strBagControl = m_strBagControlTable;
            frm.m_strObjectBag = m_strObjectBagTalbe;
            frm.m_strObjectElement = m_strObjectElementTable;
            frm.Initialize();

            if (DialogResult.OK == frm.ShowDialog())
            {
                m_strServer = frm.m_strServer;
                m_strID = frm.m_strID;
                m_strPassword = frm.m_strPassword;
                m_strDatabase = frm.m_strDatabase;
                m_strRegenTable = frm.m_strRegenTable;
                m_strBagControlTable = frm.m_strBagControl;
                m_strObjectBagTalbe = frm.m_strObjectBag;
                m_strObjectElementTable = frm.m_strObjectElement;

                MFramework.Instance.BreakObject.ClearSettedObjectData();

                MFramework.Instance.BreakObject.Clear();

                MFramework.Instance.BreakObject.m_nMapNum = nMapNumber;
                MFramework.Instance.BreakObject.m_strXmlPath = m_strXmlPath.ToString();

                LoadMonsterBagControl();
                LoadObjectBagFromDB();
                LoadObjectElementFromDB();
                LoadMapMonsterRegenFromDB(nMapNumber);
                SetSettedBagList();

                SyncObjectBag();
                SyncObjectElement();

                SaveConfig();
            }
        }

        private void btnSaveDB_Click(object sender, EventArgs e)
        {
            DialogResult result = MessageBox.Show(
                "서버에 데이타를 올립니다.", "데이타 업로드", MessageBoxButtons.OKCancel);

            if(result == DialogResult.OK)
            {
                int nMapNo = 0;
                bool bCheck = int.TryParse(tbMapNumber.Text, out nMapNo);
                if (!bCheck || nMapNo == 0)
                    return;

                if(SaveObjectElementToDB())
                {
                    if (SaveObjectBagToDB())
                    {
                        SaveMapMonsterRegenToDB(nMapNo);
                    }
                }
                

                //SyncSettedBreakObjectList();
            }
        }

        private void btnRefresh_Click(object sender, EventArgs e)
        {
            SyncObjectBag();
            SyncObjectElement();
        }

        private void tbXmlPath_TextChanged(object sender, EventArgs e)
        {
            m_strXmlPath = tbXmlPath.Text;
            MFramework.Instance.BreakObject.m_strXmlPath = m_strXmlPath.ToString();
        }

        private void btnElementRemove_Click(object sender, EventArgs e)
        {
            int nIndex = listObjectElement.SelectedIndex;
            if(nIndex < 0)
                return;

            DialogResult result = MessageBox.Show(
                "Element를 지우시겠습니까?", "내용 삭제", MessageBoxButtons.OKCancel);
            if (result == DialogResult.OK)
            {
                MFramework.Instance.BreakObject.RemoveElement(nIndex);
                SyncObjectElement();
            }
        }

        private void listObjectBag_SelectedIndexChanged(object sender, EventArgs e)
        {
            int nIndex = listObjectBag.SelectedIndex;
            if (nIndex < 0)
                return;

            MFramework.Instance.BreakObject.ObjectBagSelect(nIndex);

            tbGroupName.Text = MFramework.Instance.BreakObject.m_pMakingData.m_nBagNo.ToString();
            tbRegenTime.Text = MFramework.Instance.BreakObject.m_pMakingData.m_nRegenPeriod.ToString();
            tbGroupNum.Text = MFramework.Instance.BreakObject.m_pMakingData.m_nPointGroup.ToString();
            listObject.Items.Clear();

            int nCount = MFramework.Instance.BreakObject.m_pMakingData.m_pmElementList.Count;
            for (int nn = 0; nn < nCount; ++nn)
            {
                ObjectElementData kData = (ObjectElementData)MFramework.Instance.BreakObject.m_pMakingData.m_pmElementList[nn];
                string strText = "ENum:" + kData.m_nElementNo + " ONum:" + kData.m_nObjectNo +
                    " X:" + kData.m_fRelativeX + " Y:" + kData.m_fRelativeY + " Z:" + kData.m_fRelativeZ;
                listObject.Items.Add(strText);
            }
        }

        private void listObjectElement_SelectedIndexChanged(object sender, EventArgs e)
        {
            int nIndex = listObjectElement.SelectedIndex;
            if (nIndex < 0)
                return;

            int nPos = 0;
            for(int nn=0; nn<MFramework.Instance.BreakObject.m_pmSettedElementList.Count; ++nn)
            {
                ObjectElementData kData = (ObjectElementData)MFramework.Instance.BreakObject.m_pmSettedElementList[nn];
                if (kData.m_nChangeFlag == 3)
                    continue;

                if(nPos == nIndex)
                {
                    tbElementNo.Text = kData.m_nElementNo.ToString();
                    tbObjectNumber.Text = kData.m_nObjectNo.ToString();
                    tbPosX.Text = kData.m_fRelativeX.ToString();
                    tbPosY.Text = kData.m_fRelativeY.ToString();
                    tbPosZ.Text = kData.m_fRelativeZ.ToString();
                    return;
                }

                ++nPos;
            }
        }

        private void listObject_SelectedIndexChanged(object sender, EventArgs e)
        {
            int nIndex = listObject.SelectedIndex;
            if (nIndex < 0)
                return;

            MakingData kData = (MakingData)MFramework.Instance.BreakObject.m_pMakingData;
            ObjectElementData kElement = (ObjectElementData)kData.m_pmElementList[nIndex];
            tbElementNo.Text = kElement.m_nElementNo.ToString();
            tbObjectNumber.Text = kElement.m_nObjectNo.ToString();
            tbPosX.Text = kElement.m_fRelativeX.ToString();
            tbPosY.Text = kElement.m_fRelativeY.ToString();
            tbPosZ.Text = kElement.m_fRelativeZ.ToString();
        }

        private void tbRegenTime_TextChanged(object sender, EventArgs e)
        {
            int nRegenTime;
            Int32.TryParse(tbRegenTime.Text, out nRegenTime);
            MFramework.Instance.BreakObject.m_pMakingData.m_nRegenPeriod = nRegenTime;
        }

        private void btnObjectBagRemove_Click(object sender, EventArgs e)
        {
            int nIndex = listObjectBag.SelectedIndex;
            if (nIndex < 0)
                return;

            DialogResult result = MessageBox.Show(
                "ObjectBag을 지우시겠습니까?", "내용 삭제", MessageBoxButtons.OKCancel);
            if (result == DialogResult.OK)
            {
                MFramework.Instance.BreakObject.RemoveObjectBag(nIndex);
                SyncObjectBag();
            }
        }

        private void tbGroupNum_TextChanged(object sender, EventArgs e)
        {
            int iGroupNum;
            Int32.TryParse(tbGroupNum.Text, out iGroupNum);
            MFramework.Instance.BreakObject.m_pMakingData.m_nPointGroup = iGroupNum;
        }
    }

    public class BreakObjectData : Object
    {
        #region Private Data
        public Guid m_kGuid = Guid.Empty;
        public string m_strMemo = null;
        public int m_iMapNo = 0;
        public int m_iParentBagType = 0;
        public int m_iMonParentBagNo = 0;
        public int m_iPointGroup = 0;
        public int m_iRegenPeriod = 0;
        public int m_iMoveRange = 100;
        public float m_fPosX = 0.0f;
        public float m_fPosY = 0.0f;
        public float m_fPosZ = 0.0f;
        public string m_strGroupNum = "0";
        public MEntity m_pkEntity;

        // Monster Bag
        public int[] m_aiMonsterID = new int[10];
        public int[] m_aiMonsterRate = new int[10];
        #endregion

        public void Initilaize(Guid kGuid, string strMemo, int iMapNo, int iParentBagType, int iMonParentBagNo,
            int iPointGroup, int iRegenPeriod, float fPosX, float fPosY, float fPosZ, int iMoveRange)
        {
            m_kGuid = kGuid;
            m_strMemo = strMemo;
            m_iMapNo = iMapNo;
            m_iPointGroup = iPointGroup;
            m_iParentBagType = iParentBagType;
            m_iMonParentBagNo = iMonParentBagNo;
            m_iPointGroup = iPointGroup;
            m_iRegenPeriod = iRegenPeriod;
            m_fPosX = fPosX;
            m_fPosY = fPosY;
            m_fPosZ = fPosZ;
            m_iMoveRange = iMoveRange;
        }
    }
}