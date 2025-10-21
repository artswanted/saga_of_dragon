using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Data.SqlClient;
using System.Xml;
using System.IO;
using System.Data.OleDb;
using System.Collections;
using System.Diagnostics;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;
using Emergent.Gamebryo.SceneDesigner.Framework;

namespace Emergent.Gamebryo.SceneDesigner.GUI.ProjectG
{
	public partial class MonsterPanel : Form
	{
        const int MONSTERBAGLISTVIEW_EDITBOX = 1;

		#region Private Data
		private IUICommandService m_uiCommandService;
		private ArrayList m_pmSettedMonsterList = new ArrayList();		  //�ߺ��� �ִ� ����Ʈ
		private ArrayList m_pmMonsterList = new ArrayList();				// �ߺ��� ����
		// MonsterBag
		private ArrayList m_pmMonsterBagList = new ArrayList();				// �ߺ��� ���� ���� ��
        private Hashtable m_pmMonsterBagControlList = new Hashtable();      // ���� ��Ʈ�� ����Ʈ
		// ������Ʈ ���� �μ�Ʈ ���� ���ϱ� ���� �����صδ� �ε���
		private ArrayList m_pmMonsterGuidListFromServer = new ArrayList();
		private int m_iMapNo = 0;
		private MonsterData m_pkActiveMonsterData = new MonsterData();
		private string m_strServer = "211.232.145.177,2433";
		private string m_strID = "gamedesign";
		private string m_strPassword = "";
		private string m_strDatabase = "DR2_Def";
		private string m_strMonsterTable = "TB_DefMonster";
		private string m_strRegenTable = "TB_DefMapMonsterRegen_01";
		private string m_strBagControlTable = "TB_DefMonsterBagControl";
		MEntity m_pkSelectedEntity;
		private string m_strXmlPath = "";

		//clasno.xml relative
		private XmlManager m_kClassNoXmlManager = new XmlManager();
		private XmlDocument m_kClassNoXmlDoc = new XmlDocument();
		private XmlNodeList m_kClassNoXmlNodelist;
		//path.xml relative
		private XmlManager m_kPathXmlManager = new XmlManager();
		private XmlDocument m_kPathXmlDoc = new XmlDocument();
		private XmlNodeList m_kPathXmlNodelist;

		private OleDbConnection m_kDBConn = null;
		#endregion

		public MonsterPanel()
		{
			InitializeComponent();

			UICommandService.BindCommands(this);

			LoadConfig();
			SaveConfig();

			m_cbGroupNum.Items.Add("All");
            m_cbGroupNum.Items.Add("Default");
			m_cbGroupNum.Items.Add("Level 1");
			m_cbGroupNum.Items.Add("Level 2");
			m_cbGroupNum.Items.Add("Level 3");
			m_cbGroupNum.Items.Add("Level 4");
            m_cbGroupNum.Items.Add("Level 5");
            m_cbGroupNum.Items.Add("Level 6");

			m_cbGroupNum.SelectedItem = "All";


			//bool bCheck = LoadMonsterList();

			//if (m_tbMapNo != null &&
			//	bCheck)
			//{
			//	int iMapNo;
			//	bool bAble = Int32.TryParse(m_tbMapNo.Text, out iMapNo);
			//	if (bAble)
			//	{
			//		LoadMonsterFromMap(Int32.Parse(m_tbMapNo.Text));
			//		RefreshSettedMonsterList();
			//	}
			//}
		}

		private string GetMonsterIDFromBagNo(int iBagNo)
		{//���� ���ȣ�� ������ ID�� ���´�. 
			//1 TB_DefMonsterBagElements ���� ���ȣ�� ���� ��ȣ�� ����ִ� ���̺����� ���ȣ�� ���ʷ� ���� ��ȣ�� ���´�.

            if (true == m_cbAlteredMonsterShape.Checked)
            {
                return "[Default]monster_target";
            }

			string source =
				"Provider=SQLOLEDB;" +
				"server=" + m_strServer + ";" +
				"uid=" + m_strID + ";pwd=" + m_strPassword + ";" +
				"database=" + "DR2_Def";
			string select = "SELECT MonNo01" +
				" FROM TB_DefMonsterBagElements WHERE BagNo=" + iBagNo;
			string strResult = null;
			int iMonNo = 0;
			int iCount = 0;

			if (null == m_kDBConn) m_kDBConn = new OleDbConnection(source);
			OleDbDataReader aReader = null;
			if (m_kDBConn == null)
			{
				MessageBox.Show("DB������ ��ȿ���� �ʽ��ϴ�. (��ġ : GetMonsterIDFromBagNo())");
				return "false";
			}

			try
			{
				if (ConnectionState.Open != m_kDBConn.State)
				{
					m_kDBConn.Open();
				}
				OleDbCommand cmd = new OleDbCommand(select, m_kDBConn);
				aReader = cmd.ExecuteReader();
				if (aReader.FieldCount > 0)
				{
					while (aReader.Read())
					{
						++iCount;
						int iMonNoOrd = aReader.GetOrdinal("MonNo01");
						iMonNo = aReader.GetInt32(iMonNoOrd);
					}
				}
			}
			catch (System.Exception e)
			{
				m_lState.Text = m_strServer + " Disconnected.";
				MessageBox.Show(e.ToString());
				return null;
			}
			finally
			{
				try
				{
					if (aReader != null)
						aReader.Close();
					m_kDBConn.Close();
					m_kDBConn = null;
				}
				catch (System.Exception e)
				{
					MessageBox.Show(e.ToString());
				}
			}

			if (0 == iCount)
			{
				return null;
			}

			
			if((null==m_kClassNoXmlNodelist) || (null==m_kPathXmlNodelist))
			{//���� xml������ �ε�Ǿ����� ������ �ε��Ѵ�.
				LoadXml();
			}

			//2 ���� ��ȣ(iMonNo)�� ���ʷ� xmlpath/classno.xml���� ID�� ���´�.
			//string strPath = m_strXmlPath + "\\" + "classno.xml";
			for (int iIndex = 0; iIndex < m_kClassNoXmlNodelist.Count; ++iIndex)
			{
				XmlElement child = (XmlElement)m_kClassNoXmlNodelist.Item(iIndex);
				if (child == null) continue;
				string strItemNo = child.GetAttribute("NO");
				if (strItemNo != null && strItemNo.Equals(iMonNo.ToString()))
				{
					strResult = child.GetAttribute("ID");
					//strResult = strResult.Replace(" ", "");
					break;
				}
			}
			//3 ������ ���� ID(strResult)�� ���ʷ� xmlpath/path.xml���� monster xml path�� ���´�.
			for (int iIndex = 0; iIndex < m_kPathXmlNodelist.Count; ++iIndex)
			{
				XmlElement child = (XmlElement)m_kPathXmlNodelist.Item(iIndex);
				if (child == null) continue;
				string strID = child.GetAttribute("ID");
				if (strID != null && strID.Equals(strResult))
				{
					strResult = child.InnerText;
					//strResult = strResult.Replace(" ", "");
					break;
				}
			}
			//4 ������ ���� monster xml path���� �������(���丮, ������, Ȯ����)�� �����ϸ� ������ �ȷ�Ʈ ���ø��̸��� ����
			//...�� ������ nif�� �����鼭 �ؽ��ĸ� �ٸ��� ���� ��쵵 �ִٰ� ��.
			//monster xml path�� ��� xml���� �ȿ� �ؽ��ļҽ� ������ �ִ��� Ȯ�� �� ������ ������� ���� ������ �ؽ��ĸ� ��ü�ؾ���.
			//���� ���Ͱ� ����� �����Ը� �غ���.
			XmlManager kXmlManager = new XmlManager();
			string strMonsterXmlPath = m_strXmlPath + "\\" + strResult;
			kXmlManager.XmlFileLoad(strMonsterXmlPath);
			XmlDocument kXmlDoc = new XmlDocument();
			kXmlDoc = kXmlManager.GetDocument();
			XmlNodeList kXmlNodeList = kXmlDoc.GetElementsByTagName("KFMPATH");
			for (int iIndex = 0; iIndex < kXmlNodeList.Count; ++iIndex)
			{
				XmlElement child = (XmlElement)kXmlNodeList.Item(iIndex);
				if (child == null) continue;
				strResult = child.InnerText;
				//strResult = strResult.Replace(" ", "");
			}

			int iIndexOfLinefeed = strResult.LastIndexOf("/");
			int iLength = strResult.Length;
			strResult = strResult.Substring(iIndexOfLinefeed + 1);

			int iIndexOfDot = strResult.LastIndexOf(".kfm");
			iLength = strResult.Length;
			strResult = strResult.Substring(0, iIndexOfDot);

			return strResult;
		}

		private void BuildMonsterTemplateTable()
		{//������ ���ȣ�� ����ID���̺��� �����Ѵ�.
			string source =
				"Provider=SQLOLEDB;" +
				"server=" + "211.232.145.177,2433" + ";" +
				"uid=" + "gamedesign" + ";pwd=" + "tldzmaktmxj!@#" + ";" +
				"database=" + "DR2_Def";
			string select = "SELECT ParentBagNo, ParentBagType" + " FROM TB_DefMapMonsterRegen_01";
			string strResult = null;
			int iBagNo = 0;

			if (null == m_kDBConn) m_kDBConn = new OleDbConnection(source);
			OleDbDataReader aReader = null;
			if (m_kDBConn == null)
			{
				MessageBox.Show("DB������ ��ȿ���� �ʽ��ϴ�. (��ġ : GetMonsterIDFromBagNo())");
				return;
			}

			try
			{
				if (ConnectionState.Open != m_kDBConn.State)
				{
					m_kDBConn.Open();
				}
				OleDbCommand cmd = new OleDbCommand(select, m_kDBConn);
				aReader = cmd.ExecuteReader();
				int nCount = 0;
				if (aReader.FieldCount > 0)
				{
					while (aReader.Read())
					{
						++nCount;
						int iBagNoOrd = aReader.GetOrdinal("ParentBagNo");
						iBagNo = aReader.GetInt32(iBagNoOrd);
						int iBagTypeOrd = aReader.GetOrdinal("ParentBagType");
						int iBagType = aReader.GetByte(iBagTypeOrd);
						if (1 == iBagType)
						{//Ÿ���� ������ ����.
							MFramework.Instance.Monster.AddMonsterTemplate(iBagNo, GetMonsterIDFromBagNo(iBagNo));
						}
					}
				}
			}
			catch (System.Exception e)
			{
				m_lState.Text = m_strServer + " Disconnected.";
				MessageBox.Show(e.ToString());
				return;
			}
			finally
			{
				try
				{
					if (aReader != null)
						aReader.Close();
					m_kDBConn.Close();
					m_kDBConn = null;
				}
				catch (System.Exception e)
				{
					MessageBox.Show(e.ToString());
				}
			}
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

		private static ISelectionService ms_pmSelectionService = null;
		private static ISelectionService SelectionService
		{
			get
			{
				if (ms_pmSelectionService == null)
				{
					ms_pmSelectionService = ServiceProvider.Instance
						.GetService(typeof(ISelectionService)) as
						ISelectionService;
					Debug.Assert(ms_pmSelectionService != null,
						"Selection service not found!");
				}
				return ms_pmSelectionService;
			}
		}

		private void LoadConfig()
		{
			if (MFramework.Instance.StreamManager.LoadConfig("config.txt"))
			{
				string strServer = MFramework.Instance.StreamManager.ServerIP;
				string strID = MFramework.Instance.StreamManager.ServerID;
				string strDatabase = MFramework.Instance.StreamManager.ServerDatabase;
				string strMonsterTable = MFramework.Instance.StreamManager.ServerMonsterTable;
				string strRegenTable = MFramework.Instance.StreamManager.ServerRegenTable;
				string strXmlPath = MFramework.Instance.StreamManager.XmlPath;

				if (strServer.Length > 0)
					m_strServer = strServer;
				if (strID.Length > 0)
					m_strID = strID;
				if (strDatabase.Length > 0)
					m_strDatabase = strDatabase;
				if (strMonsterTable.Length > 0)
					m_strMonsterTable = strMonsterTable;
				if (strRegenTable.Length > 0)
					m_strRegenTable = strRegenTable;
				if (strXmlPath.Length > 0)
					m_tbXmlPath.Text = strXmlPath;
			}
		}

		private void SaveConfig()
		{
			MFramework.Instance.StreamManager.ServerIP = m_strServer;
			MFramework.Instance.StreamManager.ServerID = m_strID;
			MFramework.Instance.StreamManager.ServerDatabase = m_strDatabase;
			MFramework.Instance.StreamManager.ServerMonsterTable = m_strMonsterTable;
			MFramework.Instance.StreamManager.ServerRegenTable = m_strRegenTable;
			MFramework.Instance.StreamManager.XmlPath = m_tbXmlPath.Text;
			MFramework.Instance.StreamManager.SaveConfig("config.txt");
		}

		// ������ �ִ� ���͸� �ε�.
		private bool LoadMonsterList()
		{
			return false;
			/*
						m_pmMonsterList.Clear();
						m_lbMonsterList.Items.Clear();

						string source =
							"Provider=SQLOLEDB;" +
							"server=" + m_strServer + ";" +
							"uid=" + m_strID + ";pwd=" + m_strPassword + ";" +
							"database=" + m_strDatabase;
						string select = "SELECT MonsterNo,Memo " +
							" FROM " + m_strMonsterTable;

						if(null == m_kDBConn) m_kDBConn = new OleDbConnection(source);
						OleDbDataReader aReader = null;
						if (m_kDBConn == null)
						{
							MessageBox.Show("Connection is null");
							return false;
						}
			
						try
						{
							m_kDBConn.Open();
							OleDbCommand cmd = new OleDbCommand(select, m_kDBConn);
							aReader = cmd.ExecuteReader();
							if (aReader.FieldCount > 0)
							{
								int iMonsterNoOrdinal = aReader.GetOrdinal("MonsterNo");
								int iMemoOrdinal = aReader.GetOrdinal("Memo");
								while (aReader.Read())
								{
									int iMonsterNo = aReader.GetInt32(iMonsterNoOrdinal);
									String strMemo = aReader.GetString(iMemoOrdinal);

									string strData =
										"No." +
										iMonsterNo +
										" " +
										strMemo;

									m_lbMonsterList.Items.Add(strData);

									MonsterData kMonsterData = new MonsterData();
									kMonsterData.m_iMonParentBagNo = iMonsterNo;
									kMonsterData.m_strMemo = strMemo;
									m_pmMonsterList.Add(kMonsterData);
								}
							}
						}
						catch (System.Exception e)
						{
							m_lState.Text = m_strServer + " Disconnected.";
							MessageBox.Show(e.ToString());
							return false;
						}
						finally
						{
							try
							{
								if (aReader != null)
									aReader.Close();
								m_kDBConn.Close();
							}
							catch (System.Exception e)
							{
								MessageBox.Show(e.ToString());
							}
						}

						m_lState.Text = m_strServer + " Connected.";
						return true;
			 */
		}

		// ������ �ִ� ���� ���� �ε�.
		private bool LoadMonsteBagList()
		{
			return false;

			//m_pmMonsterBagList.Clear();
			//m_lbMonsterBagList.Items.Clear();

			//string source =
			//    "Provider=SQLOLEDB;" +
			//    "server=" + m_strServer + ";" +
			//    "uid=" + m_strID + ";pwd=" + m_strPassword + ";" +
			//    "database=" + m_strDatabase;
			//string select = "SELECT BagNo,Memo,MonNo1,Rate1,MonNo2,Rate2,MonNo3,Rate3,MonNo4,Rate4,MonNo5,Rate5,MonNo6,Rate6,MonNo7,Rate7,MonNo8,Rate8,MonNo9,Rate9,MonNo10,Rate10 " +
			//    " FROM " + "TB_DefMonsterBag";

			//if(null == m_kDBConn) m_kDBConn = new OleDbConnection(source);
			//OleDbDataReader aReader = null;
			//if (m_kDBConn == null)
			//{
			//    MessageBox.Show("Connection is null");
			//    return false;
			//}

			//try
			//{
			//    m_kDBConn.Open();
			//    OleDbCommand cmd = new OleDbCommand(select, m_kDBConn);
			//    aReader = cmd.ExecuteReader();
			//    if (aReader.FieldCount > 0)
			//    {
			//        int iBagNoOrdinal = aReader.GetOrdinal("BagNo");
			//        int iMemoOrdinal = aReader.GetOrdinal("Memo");
			//        int[] aiMonsterIDOrdinal = new int[10];
			//        int[] aiMonsterRateOrdinal = new int[10];

			//        for (int i = 0; i < 10; i++)
			//        {
			//            string strMonNo = "MonNo";
			//            string strRate = "Rate";
			//            strMonNo += (i+1).ToString();
			//            strRate += (i + 1).ToString();
			//            aiMonsterIDOrdinal[i] = aReader.GetOrdinal(strMonNo);
			//            aiMonsterRateOrdinal[i] = aReader.GetOrdinal(strRate);
			//        }

			//        while (aReader.Read())
			//        {
			//            MonsterData kMonsterData = new MonsterData();

			//            int iBagNo = aReader.GetInt32(iBagNoOrdinal);
			//            String strMemo = aReader.GetString(iMemoOrdinal);

			//            for (int i = 0; i < 10; i++)
			//            {
			//                int iMonsterID = aReader.GetInt32(aiMonsterIDOrdinal[i]);
			//                int iMonsterRate = aReader.GetInt32(aiMonsterRateOrdinal[i]);
			//                kMonsterData.m_aiMonsterID[i] = iMonsterID;
			//                kMonsterData.m_aiMonsterRate[i] = iMonsterRate;
			//            }

			//            string strData =
			//                "No." +
			//                iBagNo +
			//                " " +
			//                strMemo;
			//            m_lbMonsterBagList.Items.Add(strData);

			//            kMonsterData.m_iMonParentBagNo = iBagNo;
			//            kMonsterData.m_strMemo = strMemo;
			//            m_pmMonsterBagList.Add(kMonsterData);
			//            MFramework.Instance.Monster.AddMonsterBagData(
			//                kMonsterData.m_iMonParentBagNo,
			//                kMonsterData.m_strMemo,
			//                kMonsterData.m_aiMonsterID,
			//                kMonsterData.m_aiMonsterRate);
			//        }
			//    }

			//    Directory.CreateDirectory(Directory.GetCurrentDirectory() + "\\" + "Temp");

			//    string strAutoSaveFileName = DateTime.Now.ToString() + ".bag";
			//    strAutoSaveFileName = strAutoSaveFileName.Replace(' ', '_');
			//    strAutoSaveFileName = strAutoSaveFileName.Replace(':', '_');
			//    strAutoSaveFileName = Directory.GetCurrentDirectory() + "\\" + "Temp\\" + strAutoSaveFileName;
			//    SaveBagData(strAutoSaveFileName);
			//}
			//catch (System.Exception e)
			//{
			//    m_lState.Text = m_strServer + " Disconnected.";
			//    MessageBox.Show(e.ToString());
			//    return false;
			//}
			//finally
			//{
			//    try
			//    {
			//        if (aReader != null)
			//            aReader.Close();
			//        m_kDBConn.Close();
			//    }
			//    catch (System.Exception e)
			//    {
			//        MessageBox.Show(e.ToString());
			//    }
			//}

			//m_lState.Text = m_strServer + " Connected.";
			//return true;
		}

		// ��ġ �� ���� �ε�
		private bool LoadMonsterFromMap(int iMapNo)
		{
			m_pmMonsterGuidListFromServer.Clear();
			m_pmSettedMonsterList.Clear();

			string source =
				"Provider=SQLOLEDB;" +
				"server=" + m_strServer + ";" +
				"uid=" + m_strID + ";pwd=" + m_strPassword + ";" +
				"database=" + m_strDatabase;
			string select = "SELECT PointGuid,Memo,MapNo,ParentBagType,ParentBagNo,PointGroup,RegenPeriod,PosX,PosY,PosZ,MoveRange,Direction " +
				" FROM " + m_strRegenTable;// WHERE MapNo=" + iMapNo;
			bool bNotException = true;

			if (null == m_kDBConn) m_kDBConn = new OleDbConnection(source);
			OleDbDataReader aReader = null;
			if (m_kDBConn == null)
			{
				MessageBox.Show("Connection is null");
				return false;
			}

			try
			{
				if (ConnectionState.Open != m_kDBConn.State)
				{
					m_kDBConn.Open();
				}
				OleDbCommand cmd = new OleDbCommand(select, m_kDBConn);
				aReader = cmd.ExecuteReader();
				int nCount = 0;
				if (aReader.FieldCount > 0)
				{
					while (aReader.Read())
					{
						++nCount;
						int iMapNoOrd = aReader.GetOrdinal("MapNo");
						int iMapCheckNo = aReader.GetInt32(iMapNoOrd);
						int iPointGuid = aReader.GetOrdinal("PointGuid");
						int iParentBagTypeOrd = aReader.GetOrdinal("ParentBagType");// 1 ���� 2 �ı�������Ʈ
						byte byParentBatType = aReader.GetByte(iParentBagTypeOrd);
						if (iMapCheckNo == iMapNo && byParentBatType == 1)
						{
							// �ε�
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

							MonsterData kData = new MonsterData();
							//Random kRand = new Random();

							kData.Initilaize(
								aReader.GetGuid(iPointGuid),
								strMemo,
								iMapCheckNo,
								aReader.GetInt32(iMonParentBagNoOrd),
								aReader.GetInt32(iPointGroupOrd),
								aReader.GetInt32(iRegenPeriodOrd),
								(float)aReader.GetDouble(iPosXOrd),
								(float)aReader.GetDouble(iPosYOrd),
								(float)aReader.GetDouble(iPosZOrd),
								(float)aReader.GetByte(iRotAxZOrd),
								aReader.GetInt32(iMoveRange)
							);

							m_pmSettedMonsterList.Add(kData);
						}

						m_pmMonsterGuidListFromServer.Add(aReader.GetGuid(iPointGuid));
					}
				}

				nCount = nCount;
			}
			catch (System.Exception e)
			{
				m_lState.Text = m_strServer + " Disconnected.";
				MessageBox.Show(e.ToString());
				bNotException = false;
			}
			finally
			{
				try
				{
					if (aReader != null)
						aReader.Close();
					m_kDBConn.Close();
				}
				catch (System.Exception e)
				{
					MessageBox.Show(e.ToString());
				}
			}

			m_lState.Text = m_strServer + " Connected.";
			return true;
		}

		private bool LoadMonsterBagControl()
		{
			string source =
				"Provider=SQLOLEDB;" +
				"server=" + m_strServer + ";" +
				"uid=" + m_strID + ";pwd=" + m_strPassword + ";" +
				"database=" + m_strDatabase;
			string select = "SELECT ParentBagNo,DefaultBagNo,Level01BagNo,Level02BagNo,Level03BagNo,Level04BagNo,Level05BagNo,Level06BagNo" +
				" FROM " + m_strBagControlTable;// WHERE MapNo=" + iMapNo;
			bool bNotException = true;

			if (null == m_kDBConn) m_kDBConn = new OleDbConnection(source);
			OleDbDataReader aReader = null;
			if (m_kDBConn == null)
			{
				MessageBox.Show("Connection is null");
				return false;
			}

			try
			{
				if (ConnectionState.Open != m_kDBConn.State)
				{
					m_kDBConn.Open();
				}
				OleDbCommand cmd = new OleDbCommand(select, m_kDBConn);
				aReader = cmd.ExecuteReader();
				if (aReader.FieldCount > 0)
				{
                    m_pmMonsterBagControlList.Clear();
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

                        m_pmMonsterBagControlList.Add(kData.m_iParentBagNo,kData);
					}
                    MFramework.Instance.Monster.SetMonsterBagControl(m_pmMonsterBagControlList);
				}
			}
			catch (System.Exception e)
			{
				m_lState.Text = m_strServer + " Disconnected.";
				MessageBox.Show(e.ToString());
				bNotException = false;
			}
			finally
			{
				try
				{
					if (aReader != null)
						aReader.Close();
					m_kDBConn.Close();
                    m_kDBConn = null;
				}
				catch (System.Exception e)
				{
					MessageBox.Show(e.ToString());
				}
			}

			m_lState.Text = m_strServer + " Connected.";
			return bNotException;
		}

		private void SaveMonsterDataToServer(int iMapNo)
		{
			string source =
			"server=" + m_strServer + ";" +
			"uid=" + m_strID + ";pwd=" + m_strPassword + ";" +
			"database=" + m_strDatabase;

			SqlConnection DBConn = new SqlConnection(source);

			try
			{
				if (ConnectionState.Open != DBConn.State)
				{
					DBConn.Open();
				}

				SqlCommand command;

				int iRowEffected;

				for (int i = 0; i < m_pmSettedMonsterList.Count; i++)
				{
					MonsterData kData = (MonsterData)m_pmSettedMonsterList[i];
					if (kData.m_pkEntity != null)
					{
						// �ֽ� ��ǥ ����.
                        MPoint3 kPoint = MFramework.Instance.Monster.GetMonsterPos(kData.m_pkEntity);
						kData.m_fPosX = kPoint.X;
						kData.m_fPosY = kPoint.Y;
						kData.m_fPosZ = kPoint.Z;
						kData.m_fRotAxZ = MFramework.Instance.Monster.GetMonsterRotAxZ(kData.m_pkEntity);
                        if (false == kData.IsChange())
                        {
                            continue;
                        }
					}
					else
					{
						MessageBox.Show("Entity is null");
						continue;
					}

					// Delete Check
					bool bIsDeleted = false;
					bool bDeleteNotCommit = false;
					int iCount = MFramework.Instance.Monster.m_pmDeletedMonsterList.Count;
					for (int j = 0; j < iCount; j++)
					{
						Guid kGuid = MFramework.Instance.Monster.GetDeletedMonsterGuidData(j);
						if (kData.m_kGuid == kGuid)
						{
							bIsDeleted = true;
							MFramework.Instance.Monster.m_pmDeletedMonsterList.Remove(
								MFramework.Instance.Monster.m_pmDeletedMonsterList[j]
								);

							bool bIsServer = false;
							for (int k = 0; k < m_pmMonsterGuidListFromServer.Count; k++)
							{
								if (m_pmMonsterGuidListFromServer[k].Equals(kData.m_kGuid))
								{
									m_pmMonsterGuidListFromServer.RemoveAt(k);
									bIsServer = true;
									break;
								}
							}

							MFramework.Instance.Monster.DeleteToDeletedMonsterGuidData(kGuid);

							if (!bIsServer)
							{
								bDeleteNotCommit = true;
							}

							break;
						}
					}
					if (bDeleteNotCommit)
					{
						m_pmSettedMonsterList.Remove(kData);
						i -= 1;
						continue;
					}

					// Delete Check �� ��������� Update���� üũ.
					bool bIsUpdate = false;
					if (!bIsDeleted)
					{
						for (int j = 0; j < m_pmMonsterGuidListFromServer.Count; j++)
						{
							if (m_pmMonsterGuidListFromServer[j].Equals(kData.m_kGuid))
							{
								j = m_pmMonsterGuidListFromServer.Count;
								bIsUpdate = true;
							}
						}
					}

					int nUpdateType;
					if (bIsDeleted) // Deleted
					{
						//command = new SqlCommand(
						//    "DELETE FROM " + m_strRegenTable + " " +
						//    "WHERE PointGuid='" + kData.m_kGuid + "'",
						//    DBConn);
						nUpdateType = 3;
					}
					else if (bIsUpdate) // Updated
					{
						//command = new SqlCommand(
						//    "UPDATE " + m_strRegenTable + " SET PointGuid = @PointGuid, " +
						//    "Memo = @Memo, " +
						//    "MapNo = @MapNo, " +
						//    "ParentBagType = @ParentBagType, " +
						//    "ParentBagNo = @ParentBagNo, " +
						//    "PointGroup = @PointGroup, " +
						//    "RegenPeriod = @RegenPeriod, " +
						//    "PosX = @PosX, " +
						//    "PosY = @PosY, " +
						//    "PosZ = @PosZ, " +
						//    "MoveRange = @MoveRange " +
						//    "WHERE PointGuid='" + kData.m_kGuid + "'",
						//    DBConn);
						nUpdateType = 2;
					}
					else // Insert
					{
						//command = new SqlCommand(
						//    "INSERT INTO " + m_strRegenTable +
						//    "( PointGuid,  Memo,  MapNo,  ParentBagType,  ParentBagNo,  PointGroup,  RegenPeriod,  PosX,  PosY,  PosZ,  MoveRange ) VALUES " +
						//    "(@PointGuid, @Memo, @MapNo, @ParentBagType, @ParentBagNo, @PointGroup, @RegenPeriod, @PosX, @PosY, @PosZ, @MoveRange )", DBConn);
						nUpdateType = 1;
						m_pmMonsterGuidListFromServer.Add(kData.m_kGuid);
					}

					//if (!bIsDeleted)
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
					//    command.Parameters["@UpdateType"].Value = nUpdateType;
					//    command.Parameters["@PointGuid"].Value = kData.m_kGuid;
					//    command.Parameters["@Memo"].Value = kData.m_strMemo;
					//    command.Parameters["@MapNo"].Value = kData.m_iMapNo;
					//    command.Parameters["@ParentBagType"].Value = 1;
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
					//    command.Parameters["@UpdateType"].Value = nUpdateType;
					//    command.Parameters["@PointGuid"].Value = kData.m_kGuid;
					//}

					string strCmdText = "";
					if (!bIsDeleted)
					{
						strCmdText =
							"EXEC [dbo].[UP_MapTool_UpdateMonsterRegen] '" +
                            m_strRegenTable + "', " +
							nUpdateType + ", '" +
							kData.m_kGuid + "', '" +
							kData.m_strMemo + "', " +
							kData.m_iMapNo + ", " +
							1 + ", " +
							kData.m_iMonParentBagNo + ", " +
							kData.m_iPointGroup + ", " +
							kData.m_iRegenPeriod + ", " +
							kData.m_fPosX + ", " +
							kData.m_fPosY + ", " +
							kData.m_fPosZ + ", " +
							kData.m_iMoveRange + ", " +
							(int)kData.m_fRotAxZ;
					}
					else
					{
						strCmdText =
							"EXEC [dbo].[UP_MapTool_UpdateMonsterRegen] '" +
                            m_strRegenTable + "', " +
							nUpdateType + ", '" +
							kData.m_kGuid + "'";
					}
					
					command = new SqlCommand(strCmdText, DBConn);
					iRowEffected = command.ExecuteNonQuery();

					if (iRowEffected == 0)
					{
						throw new System.Exception("effected row was 0");
					}

					if (bIsDeleted) // Deleted
					{
						m_pmSettedMonsterList.Remove(kData);
						i -= 1;
					}

                    if (false == m_pmMonsterBagControlList.ContainsKey(kData.m_iMonParentBagNo))
                    {
                        MonsterBagControl kAdd = new MonsterBagControl();
                        kAdd.m_iParentBagNo = kData.m_iMonParentBagNo;
                        kAdd.m_IsEditing = true;
                        m_pmMonsterBagControlList.Add(kAdd.m_iParentBagNo, kAdd);
                    }
				}

                foreach (DictionaryEntry entity in m_pmMonsterBagControlList)
                {
                    MonsterBagControl kData = (MonsterBagControl)entity.Value;
                    if (kData.m_IsEditing)
                    {
                        string select = "SELECT COUNT(0) FROM " + m_strBagControlTable + 
                            " WHERE ParentBagNo=" + kData.m_iParentBagNo;

                        SqlCommand cmd_select = new SqlCommand(select, DBConn);
                        iRowEffected = Convert.ToInt32(cmd_select.ExecuteScalar());
                        if (0 == iRowEffected)
                        {
                            string strMemo = "";
                            string insert = "INSERT INTO " + m_strBagControlTable +
                               "  ([Memo],[ParentBagNo],[DefaultBagNo],[Level01BagNo]" +
                               "  ,[Level02BagNo],[Level03BagNo],[Level04BagNo],[Level05BagNo]" +
                               "  ,[Level06BagNo])" +
                               "VALUES" +
                               " ('" + strMemo + "'" +
                               "," + kData.m_iParentBagNo +
                               "," + kData.m_iDefaultBagNo +
                               "," + kData.m_iEasyBagNo +
                               "," + kData.m_iNormalBagNo +
                               "," + kData.m_iHardBagNo +
                               "," + kData.m_iUltraBagNo +
                               "," + kData.m_iLevel05Bag +
                               "," + kData.m_iLevel06Bag +
                               ")";
                            command = new SqlCommand(insert, DBConn);
                            iRowEffected = command.ExecuteNonQuery();
                            if (0 == iRowEffected)
                            {
                                throw new System.Exception("MosterBagControl Insert row was 0");
                            }
                        }
                        else
                        {
                            string update = "UPDATE " + m_strBagControlTable +
                                " SET DefaultBagNo=" + kData.m_iDefaultBagNo +
                                ", Level01BagNo=" + kData.m_iEasyBagNo +
                                ", Level02BagNo=" + kData.m_iNormalBagNo +
                                ", Level03BagNo=" + kData.m_iHardBagNo +
                                ", Level04BagNo=" + kData.m_iUltraBagNo +
                                ", Level05BagNo=" + kData.m_iLevel05Bag +
                                ", Level06BagNo=" + kData.m_iLevel06Bag +
                                " WHERE ParentBagNo=" + kData.m_iParentBagNo;
                            SqlCommand cmd_update = new SqlCommand(update, DBConn);
                            iRowEffected = cmd_update.ExecuteNonQuery();
                            if (0 == iRowEffected)
                            {
                                throw new System.Exception("MosterBagControl Update row was 0");
                            }                            
                        }
                        kData.m_IsEditing = false;
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
					DBConn.Close();
				}
				catch (System.Exception e)
				{
					MessageBox.Show(e.ToString());
				}
			}

		}

		private void RefreshSettedMonsterList()
		{
			m_lbSettedMonsterList.Items.Clear();
			for (int i = 0; i < m_pmSettedMonsterList.Count; i++)
			{
				MonsterData kData = (MonsterData)m_pmSettedMonsterList[i];
				string strText =
					"Memo: " + kData.m_strMemo + "  " +
					"XYZ: " + kData.m_fPosX + ", " + kData.m_fPosY + ", " + kData.m_fPosZ + "  " +
					"MapNo: " + kData.m_iMapNo + "  " +
					"BagNo: " + kData.m_iMonParentBagNo + "  " +
					"PointGroup: " + kData.m_iPointGroup + "  " +
					"RegenPeriod: " + kData.m_iRegenPeriod + "  " +
					"Range: " + kData.m_iMoveRange + "";

				m_lbSettedMonsterList.Items.Add(strText);
			}
		}

        private void UpdateMonsterBagListView(int iParentBagNo)
        {
            if (m_pmMonsterBagControlList.ContainsKey(iParentBagNo))
            {
                MonsterBagControl kData = (MonsterBagControl)m_pmMonsterBagControlList[iParentBagNo];

                m_lvBagControl.Items[0].Checked = kData.m_iDefaultBagNo > 0 ? true : false;
                m_lvBagControl.Items[1].Checked = kData.m_iEasyBagNo > 0 ? true : false;
                m_lvBagControl.Items[2].Checked = kData.m_iNormalBagNo > 0 ? true : false;
                m_lvBagControl.Items[3].Checked = kData.m_iHardBagNo > 0 ? true : false;
                m_lvBagControl.Items[4].Checked = kData.m_iUltraBagNo > 0 ? true : false;
                m_lvBagControl.Items[5].Checked = kData.m_iLevel05Bag > 0 ? true : false;
                m_lvBagControl.Items[6].Checked = kData.m_iLevel06Bag > 0 ? true : false;

                m_lvBagControl.Items[0].SubItems[MONSTERBAGLISTVIEW_EDITBOX].Text = kData.m_iDefaultBagNo.ToString();
                m_lvBagControl.Items[1].SubItems[MONSTERBAGLISTVIEW_EDITBOX].Text = kData.m_iEasyBagNo.ToString();
                m_lvBagControl.Items[2].SubItems[MONSTERBAGLISTVIEW_EDITBOX].Text = kData.m_iNormalBagNo.ToString();
                m_lvBagControl.Items[3].SubItems[MONSTERBAGLISTVIEW_EDITBOX].Text = kData.m_iHardBagNo.ToString();
                m_lvBagControl.Items[4].SubItems[MONSTERBAGLISTVIEW_EDITBOX].Text = kData.m_iUltraBagNo.ToString();
                m_lvBagControl.Items[5].SubItems[MONSTERBAGLISTVIEW_EDITBOX].Text = kData.m_iLevel05Bag.ToString();
                m_lvBagControl.Items[6].SubItems[MONSTERBAGLISTVIEW_EDITBOX].Text = kData.m_iLevel06Bag.ToString();
            }
            else
            {
                m_lvBagControl.Items[0].Checked = false;
                m_lvBagControl.Items[1].Checked = false;
                m_lvBagControl.Items[2].Checked = false;
                m_lvBagControl.Items[3].Checked = false;
                m_lvBagControl.Items[4].Checked = false;
                m_lvBagControl.Items[5].Checked = false;
                m_lvBagControl.Items[6].Checked = false;

                int iZero = 0;
                m_lvBagControl.Items[0].SubItems[MONSTERBAGLISTVIEW_EDITBOX].Text = iZero.ToString();
                m_lvBagControl.Items[1].SubItems[MONSTERBAGLISTVIEW_EDITBOX].Text = iZero.ToString();
                m_lvBagControl.Items[2].SubItems[MONSTERBAGLISTVIEW_EDITBOX].Text = iZero.ToString();
                m_lvBagControl.Items[3].SubItems[MONSTERBAGLISTVIEW_EDITBOX].Text = iZero.ToString();
                m_lvBagControl.Items[4].SubItems[MONSTERBAGLISTVIEW_EDITBOX].Text = iZero.ToString();
                m_lvBagControl.Items[5].SubItems[MONSTERBAGLISTVIEW_EDITBOX].Text = iZero.ToString();
                m_lvBagControl.Items[6].SubItems[MONSTERBAGLISTVIEW_EDITBOX].Text = iZero.ToString();
            }
        }

		#region WinForms event handlers
		[UICommandHandler("Idle")]
		private void Application_Idle(object sender, EventArgs e)
		{
			// Monster State refresh
			MEntity[] amSelectedEntities = SelectionService.GetSelectedEntities();
			if (amSelectedEntities.Length == 1 &&
				m_pkSelectedEntity != amSelectedEntities[0])
			{
				MEntity pkEntity = amSelectedEntities[0];
				m_pkSelectedEntity = pkEntity;
				bool bFind = MFramework.Instance.Monster.GetMonsterData(pkEntity);

				if (bFind)
				{
					int iPointGroup = MFramework.Instance.Monster.GetMonsterData_PointGroup(pkEntity);
					int iRegenPeriod = MFramework.Instance.Monster.GetMonsterData_RegenPeriod(pkEntity);
					int iParentBagNo = MFramework.Instance.Monster.GetMonsterData_ParentBagNo(pkEntity);
					string strMemo = MFramework.Instance.Monster.GetMonsterData_Memo(pkEntity);

					m_tbPointGroup.Text = iPointGroup.ToString();
					m_tbRegenPeriod.Text = iRegenPeriod.ToString();
					m_tbParentBagNo.Text = iParentBagNo.ToString();
					m_tbMemo.Text = strMemo;

                    UpdateMonsterBagListView(iParentBagNo);
				}
			}

			// �׽� ������ â ����.
			if (false)
			{
				if (m_lbMonsterList.Width != (int)((this.Width - 25) * 0.5f) - 5 ||
					m_lbMonsterList.Height != this.Height - 100)
				{
					m_lbMonsterList.Width = (int)((this.Width - 25) * 0.5f) - 5;
					m_lbMonsterList.Height = (int)(this.Height - 100);
					m_lbSettedMonsterList.Height = m_lbMonsterList.Height;

					Point ptLoc = new Point();
					ptLoc.X = m_lbMonsterList.Location.X + m_lbMonsterList.Width + 10;
					ptLoc.Y = m_lbMonsterList.Location.Y;

					m_lbSettedMonsterList.Location = ptLoc;
					m_lbSettedMonsterList.Width = (int)((this.Width - 25) * 0.5f) - 5;

					// button
					ptLoc.X = m_lbMonsterList.Location.X;
					ptLoc.Y = m_lbMonsterList.Location.Y + m_lbMonsterList.Height + 8;
					m_lblMapNo.Location = ptLoc;

					ptLoc.X = ptLoc.X + m_lblMapNo.Width + 8;
					m_tbMapNo.Location = ptLoc;

					ptLoc.X = ptLoc.X + m_tbMapNo.Width + 8;
					m_btnLoad.Location = ptLoc;

					ptLoc.X = ptLoc.X + m_btnLoad.Width + 8;
					m_btnCommit.Location = ptLoc;

					//
					ptLoc.X = m_lbMonsterList.Location.X;
					ptLoc.Y = ptLoc.Y + m_btnCommit.Height;
					m_lblRegenPeriod.Location = ptLoc;

					ptLoc.X = ptLoc.X + m_lblRegenPeriod.Width + 8;
					m_tbRegenPeriod.Location = ptLoc;

					ptLoc.X = ptLoc.X + m_tbRegenPeriod.Width + 10;
					m_btnSetting.Location = ptLoc;
				}
			}

			if (MFramework.Instance.Monster.AddedMonsterData)
			{
				m_pkActiveMonsterData = new MonsterData();
                m_pkActiveMonsterData.m_kInitData = new MonsterData();

				m_pkActiveMonsterData.m_fPosX = MFramework.Instance.Monster.m_pkAddedMonsterData.m_fPosX;
				m_pkActiveMonsterData.m_fPosY = MFramework.Instance.Monster.m_pkAddedMonsterData.m_fPosY;
				m_pkActiveMonsterData.m_fPosZ = MFramework.Instance.Monster.m_pkAddedMonsterData.m_fPosZ;
				m_pkActiveMonsterData.m_iMapNo = MFramework.Instance.Monster.m_pkAddedMonsterData.m_iMapNo;
				m_pkActiveMonsterData.m_iMonParentBagNo = MFramework.Instance.Monster.m_pkAddedMonsterData.m_iMonParentBagNo;
				m_pkActiveMonsterData.m_iPointGroup = MFramework.Instance.Monster.m_pkAddedMonsterData.m_iPointGroup;
				m_pkActiveMonsterData.m_iRegenPeriod = MFramework.Instance.Monster.m_pkAddedMonsterData.m_iRegenPeriod;
				m_pkActiveMonsterData.m_iMoveRange = MFramework.Instance.Monster.m_pkAddedMonsterData.m_iMoveRange;
				m_pkActiveMonsterData.m_kGuid = MFramework.Instance.Monster.m_pkAddedMonsterData.m_kGuid;
				m_pkActiveMonsterData.m_strMemo = MFramework.Instance.Monster.m_pkAddedMonsterData.m_strMemo;
				m_pkActiveMonsterData.m_pkEntity = MFramework.Instance.Monster.m_pkAddedMonsterData.m_pkEntity;

                m_pmSettedMonsterList.Add(m_pkActiveMonsterData);
				MFramework.Instance.Monster.AddedMonsterData = false;
			}

			// DeleteMonsterData
			if (MFramework.Instance.Monster.DeletedMonsterData)
			{
				int iCount = MFramework.Instance.Monster.m_pmDeletedMonsterList.Count;
				for (int i = 0; i < iCount; i++)
				{
					Guid kGuid = MFramework.Instance.Monster.GetDeletedMonsterGuidData(i);

					bool bCheck = false;
					int iServerCount = m_pmMonsterGuidListFromServer.Count;
					for (int j = 0; j < iServerCount; j++)
					{
						Guid kDestGuid = (Guid)m_pmMonsterGuidListFromServer[j];
						if (kGuid == kDestGuid)
						{
							bCheck = true;
							break;
						}
					}

					if (!bCheck)
					{
						for (int j = 0; j < m_pmSettedMonsterList.Count; j++)
						{
							MonsterData kData = (MonsterData)m_pmSettedMonsterList[j];
							if (kData.m_kGuid == kGuid)
							{
								m_pmSettedMonsterList.Remove(kData);
								MFramework.Instance.Monster.DeleteToDeletedMonsterGuidData(kGuid);
								i -= 1;
								iCount -= 1;
								break;
							}
						}
					}
				}
				MFramework.Instance.Monster.DeletedMonsterData = false;
			}
		}

		public void Clear()
		{
			MFramework.Instance.Monster.ClearSettedMonsterData();
			MFramework.Instance.Monster.Clear();
			m_pmSettedMonsterList.Clear();
			m_pmMonsterGuidListFromServer.Clear();
			m_lbSettedMonsterList.Items.Clear();
		}

		private void m_btnLoad_Click(object sender, EventArgs e)
		{
            MPalette pkPalette = null;
            if (true == m_cbAlteredMonsterShape.Checked)
            {
                pkPalette = MFramework.Instance.PaletteManager.GetPaletteByName("Default");
            }
            else
            {
                pkPalette = MFramework.Instance.PaletteManager.GetPaletteByName("Monster");
            }

			if (pkPalette == null)
			{
				MessageBox.Show("Monster or Default �ȷ�Ʈ�� �����ϴ�.");
				return;
			}
			MonsterPanelSetting frm = new MonsterPanelSetting();
			frm.m_strServer = m_strServer;
			frm.m_strID = m_strID;
			frm.m_strPassword = "";
			frm.m_strDatabase = m_strDatabase;
			frm.m_strMonsterTable = m_strMonsterTable;
			frm.m_strRegenTable = m_strRegenTable;
			frm.m_strBagControl = m_strBagControlTable;
			frm.m_strObjectBag = "";
			frm.m_strObjectElement = "";
			frm.Initialize();

			if (DialogResult.OK == frm.ShowDialog())
			{
				m_strServer = frm.m_strServer;
				m_strID = frm.m_strID;
				m_strPassword = frm.m_strPassword;
				m_strDatabase = frm.m_strDatabase;
				m_strMonsterTable = frm.m_strMonsterTable;
				m_strRegenTable = frm.m_strRegenTable;
				m_strBagControlTable = frm.m_strBagControl;

				//m_tbMapNo.Text = "0";

				Clear();

				//LoadMonsterList();
				//bool bCheck = LoadMonsteBagList();

				//������ ���ø����� ���̺��� ����
				//�������̺� �翡 ���� �ð��� ��������. ����԰ڴ�.
				//BuildMonsterTemplateTable();

				if (!LoadMonsterBagControl())
				{
					MessageBox.Show("Failed load Monster Bag Control");
					return;
				}
				SaveConfig();

				MFramework.Instance.Monster.ClearSettedMonsterData();
				m_pmSettedMonsterList.Clear();

				if (m_tbMapNo != null)
				{
					bool bBagNo = int.TryParse(m_tbMapNo.Text, out m_iMapNo);
					if (!bBagNo)
						return;

					if (!LoadMonsterFromMap(m_iMapNo))
					{
						MessageBox.Show("Failed load Monster for current map");
						return;
					}
					//RefreshSettedMonsterList();

					for (int i = 0; i < m_pmSettedMonsterList.Count; i++)
					{
						MonsterData kData = (MonsterData)m_pmSettedMonsterList[i];
						string kMonsterID = GetMonsterIDFromBagNo(kData.m_iMonParentBagNo);
						MEntity pkEntity = MFramework.Instance.Monster.AddSettedMonsterData(
							kData.m_kGuid, kData.m_strMemo, kData.m_iMapNo, kData.m_iMonParentBagNo,
							kData.m_iPointGroup,
							kData.m_iRegenPeriod, kData.m_fPosX, kData.m_fPosY, kData.m_fPosZ, (int)kData.m_fRotAxZ,
							kData.m_iMoveRange,
							kMonsterID
							);
						if (pkEntity != null)
						{
							kData.m_pkEntity = pkEntity;

							if (pkEntity.MasterEntity.Name.Equals("[Default]monster_target"))
							{
								ListViewItem kItem = m_lvLoadFailed.Items.Add(kMonsterID);
								kItem.SubItems.Add(kData.m_iMonParentBagNo.ToString());
							}
						}
					}
				}
			}
		}

		private void m_btnCommit_Click(object sender, EventArgs e)
		{
			DialogResult result = MessageBox.Show(
				"Overwrite the data on the server. Are you sure?",
				"Overwrite data.",
				MessageBoxButtons.OKCancel);

			if (result == DialogResult.OK)
			{
				int iMapNo = 0;
				bool bCheck = int.TryParse(m_tbMapNo.Text, out iMapNo);
				if (!bCheck)
					return;

				SaveMonsterDataToServer(iMapNo);
				//RefreshSettedMonsterList();
			}
		}

        private void m_lvBagControl_SubItemClicked(object sender, ListViewEx.SubItemEventArgs e)
        {
            if (MONSTERBAGLISTVIEW_EDITBOX == e.SubItem)
            {
                m_lvBagControl.StartEditing(textBoxBagNo, e.Item, e.SubItem);
            }
        }

        private void m_lvBagControl_ItemClicked(object sender, ItemCheckedEventArgs e)
        {
            m_lvBagControl.Items[e.Item.Index].SubItems[MONSTERBAGLISTVIEW_EDITBOX].Text = m_tbParentBagNo.Text;
        }

		private void m_lbMonsterList_DoubleClick(object sender, EventArgs e)
		{
			return;

			/*
			MPalette pkPalette = MFramework.Instance.PaletteManager.GetPaletteByName("General");
			if (pkPalette == null)
			{
				MessageBox.Show("General �ȷ�Ʈ�� �����ϴ�.");
				return;
			}
			MEntity pkTemplate = pkPalette.GetEntityByName("[General]Target.monster_target");
			if (pkTemplate == null)
			{
				MessageBox.Show("[General]Target.monster_target �� �����ϴ�.");
				return;
			}
			MScene pmScene = MFramework.Instance.Scene;
			int iIdx = m_lbMonsterList.SelectedIndex;
			MonsterData kData = (MonsterData)m_pmMonsterList[iIdx];

			int iRegenPeriod=0;
			bool bCheck = int.TryParse(m_tbRegenPeriod.Text, out iRegenPeriod);
			if (!bCheck)
			{
				MessageBox.Show("RegenPeriod �� �ùٸ��� �ʽ��ϴ�.");
				return;
			}

			MFramework.Instance.PaletteManager.ActivePalette = pkPalette;

			if (MFramework.Instance.PaletteManager.ActivePalette.ActiveEntity !=
				pkTemplate)
			{
				MFramework.Instance.PaletteManager.ActivePalette.ActiveEntity = pkTemplate;
			}

			UICommand command = UICommandService.GetCommand(
				"CreateInteractionMode");
			if (command != null)
			{
				command.DoClick(this, null);
			}

			MFramework.Instance.Monster.StackSettedMonsterData(
				kData.m_strMemo, m_iMapNo, kData.m_iMonParentBagNo, iPointGroup, iRegenPeriod, kData.m_iMoveRange);
			*/
		}

		private void m_lbMonsterBagList_DoubleClick(object sender, EventArgs e)
		{
			return;

			/*
			MPalette pkPalette = MFramework.Instance.PaletteManager.GetPaletteByName("General");
			if (pkPalette == null)
			{
				MessageBox.Show("General �ȷ�Ʈ�� �����ϴ�.");
				return;
			}
			MEntity pkTemplate = pkPalette.GetEntityByName("[General]Target.monster_target");
			if (pkTemplate == null)
			{
				MessageBox.Show("[General]Target.monster_target �� �����ϴ�.");
				return;
			}
			MScene pmScene = MFramework.Instance.Scene;
			int iIdx = m_lbMonsterBagList.SelectedIndex;
			if (iIdx == -1)
			{
				MessageBox.Show("Monster Bag �� �׸��� ���õ��� �ʾҽ��ϴ�.");
				return;
			}
			MonsterData kData = (MonsterData)m_pmMonsterBagList[iIdx];

			int iRegenPeriod = 0;
			bool bCheck = int.TryParse(m_tbRegenPeriod.Text, out iRegenPeriod);
			if (!bCheck)
			{
				return;
			}

			MFramework.Instance.PaletteManager.ActivePalette = pkPalette;

			if (MFramework.Instance.PaletteManager.ActivePalette.ActiveEntity !=
				pkTemplate)
			{
				MFramework.Instance.PaletteManager.ActivePalette.ActiveEntity = pkTemplate;
			}

			UICommand command = UICommandService.GetCommand(
				"CreateInteractionMode");
			if (command != null)
			{
				command.DoClick(this, null);
			}

			MFramework.Instance.Monster.StackSettedMonsterData(
				kData.m_strMemo, m_iMapNo, kData.m_iMonParentBagNo, iRegenPeriod, kData.m_iMoveRange);
			*/
		}

		private void m_tbRegenPeriod_TextChanged(object sender, EventArgs e)
		{
			int iRegenPeriod = 0;
			bool bCheck = int.TryParse(m_tbRegenPeriod.Text, out iRegenPeriod);
			if (!bCheck)
				return;

			MFramework.Instance.Monster.StackSettedMonsterData_RegenPeriod(
				iRegenPeriod);
		}
		#endregion

		private void m_btnSetting_Click(object sender, EventArgs e)
		{
			MonsterPanelSetting frm = new MonsterPanelSetting();
			frm.m_strServer = m_strServer;
			frm.m_strID = m_strID;
			frm.m_strPassword = m_strPassword;
			frm.m_strDatabase = m_strDatabase;
			frm.m_strMonsterTable = m_strMonsterTable;
			frm.m_strRegenTable = m_strRegenTable;
			frm.m_strBagControl = m_strBagControlTable;
			frm.m_strObjectBag = "";
			frm.m_strObjectElement = "";
			frm.Initialize();

			if (DialogResult.OK == frm.ShowDialog())
			{
				m_strServer = frm.m_strServer;
				m_strID = frm.m_strID;
				m_strPassword = frm.m_strPassword;
				m_strDatabase = frm.m_strDatabase;
				m_strMonsterTable = frm.m_strMonsterTable;
				m_strRegenTable = frm.m_strRegenTable;
				m_strBagControlTable = frm.m_strBagControl;

				m_tbMapNo.Text = "0";
				MFramework.Instance.Monster.ClearSettedMonsterData();
				m_pmSettedMonsterList.Clear();
				m_pmMonsterGuidListFromServer.Clear();
				m_lbSettedMonsterList.Items.Clear();

				LoadMonsterList();
				bool bCheck = LoadMonsteBagList();
				LoadMonsterBagControl();

				{
					LoadMonsterFromMap(Int32.Parse(m_tbMapNo.Text));
					//RefreshSettedMonsterList();
				}

				SaveConfig();
			}
		}

		private void m_btnBagAdd_Click(object sender, EventArgs e)
		{
			//MonsterBagForm kForm = new MonsterBagForm();
			//kForm.m_pmMonsterList = m_pmMonsterList;
			//kForm.Initialize();

			//if (DialogResult.OK == kForm.ShowDialog())
			//{
			//    MFramework.Instance.Monster.AddMonsterBagData(
			//        kForm.m_iBagID,
			//        kForm.m_strBagMemo,
			//        kForm.m_aiMonsterID,
			//        kForm.m_aiMonsterRate);

			//    string strData =
			//        "No." +
			//        kForm.m_iBagID +
			//        " " +
			//        kForm.m_strBagMemo;
			//    m_lbMonsterBagList.Items.Add(strData);

			//    MonsterData kMonsterData = new MonsterData();
			//    kMonsterData.m_iMonParentBagNo = kForm.m_iBagID;
			//    kMonsterData.m_strMemo = kForm.m_strBagMemo;
			//    kMonsterData.m_aiMonsterID = kForm.m_aiMonsterID;
			//    kMonsterData.m_aiMonsterRate = kForm.m_aiMonsterRate;
			//    m_pmMonsterBagList.Add(kMonsterData);
			//}
		}

		private void m_btnBagModify_Click(object sender, EventArgs e)
		{
			//int iIdx = m_lbMonsterBagList.SelectedIndex;
			//if (iIdx == -1)
			//    return;
			//MonsterData kData = (MonsterData)m_pmMonsterBagList[iIdx];
			//int iMonsterBagNo = kData.m_iMonParentBagNo;

			//MonsterBagForm kForm = new MonsterBagForm();
			//kForm.m_pmMonsterList = m_pmMonsterList;
			//for (int i=0 ; i<10 ; i++)
			//{
			//    kForm.m_aiMonsterID[i] = kData.m_aiMonsterID[i];
			//    kForm.m_aiMonsterRate[i] = kData.m_aiMonsterRate[i];
			//}
			//kForm.m_iBagID = kData.m_iMonParentBagNo;
			//kForm.m_strBagMemo = kData.m_strMemo;
			//kForm.Initialize();
			//kForm.DataUpdate();

			//if (DialogResult.OK == kForm.ShowDialog())
			//{
			//    ((MonsterData)m_pmMonsterBagList[iIdx]).m_iMonParentBagNo = kForm.m_iBagID;
			//    ((MonsterData)m_pmMonsterBagList[iIdx]).m_strMemo = kForm.m_strBagMemo;
			//    ((MonsterData)m_pmMonsterBagList[iIdx]).m_aiMonsterID = kForm.m_aiMonsterID;
			//    ((MonsterData)m_pmMonsterBagList[iIdx]).m_aiMonsterRate = kForm.m_aiMonsterRate;

			//    MFramework.Instance.Monster.ModifyMonsterBagData(
			//        iIdx,
			//        kForm.m_iBagID,
			//        kForm.m_strBagMemo,
			//        kForm.m_aiMonsterID,
			//        kForm.m_aiMonsterRate);

			//    string strData =
			//        "No." +
			//        kForm.m_iBagID +
			//        " " +
			//        kForm.m_strBagMemo;
			//    m_lbMonsterBagList.Items[iIdx] = strData;
			//}
		}

		private void m_btnBagDelete_Click(object sender, EventArgs e)
		{
			//int iIdx = m_lbMonsterBagList.SelectedIndex;
			//if (iIdx == -1)
			//    return;

			//MonsterData kData = (MonsterData)m_pmMonsterBagList[iIdx];
			//MFramework.Instance.Monster.DeleteMonsterBagData(kData.m_iMonParentBagNo);
			//m_lbMonsterBagList.Items.RemoveAt(iIdx);
			//m_pmMonsterBagList.RemoveAt(iIdx);
		}

		private void SaveBagData(string strFileName)
		{
			StreamWriter sw = new StreamWriter(strFileName);
			sw.WriteLine(m_pmMonsterBagList.Count);
			for (int i = 0; i < m_pmMonsterBagList.Count; i++)
			{
				MonsterData kData = (MonsterData)m_pmMonsterBagList[i];
				sw.WriteLine(kData.m_iMonParentBagNo);
				sw.WriteLine(kData.m_strMemo);

				for (int j = 0; j < 10; j++)
				{
					sw.WriteLine(kData.m_aiMonsterID[j]);
					sw.WriteLine(kData.m_aiMonsterRate[j]);
				}
			}

			sw.Close();
		}

		private void LoadBagData(string strFileName)
		{
			//StreamReader sr = new StreamReader(strFileName);
			//int iCount = 0;
			//bool bConvert = int.TryParse(sr.ReadLine(), out iCount);

			//string strData;
			//if (bConvert)
			//{
			//    for (int i = 0; i < iCount; i++)
			//    {
			//        MonsterData kData = new MonsterData();
			//        strData = sr.ReadLine();
			//        kData.m_iMonParentBagNo = int.Parse(strData);
			//        strData = sr.ReadLine();
			//        kData.m_strMemo = strData;

			//        for (int j = 0; j < 10; j++)
			//        {
			//            strData = sr.ReadLine();
			//            kData.m_aiMonsterID[j] = int.Parse(strData);
			//            strData = sr.ReadLine();
			//            kData.m_aiMonsterRate[j] = int.Parse(strData);
			//        }

			//        m_pmMonsterBagList.Add(kData);

			//        strData =
			//            "No." +
			//            kData.m_iMonParentBagNo +
			//            " " +
			//            kData.m_strMemo;
			//        m_lbMonsterBagList.Items.Add(strData);

			//        MFramework.Instance.Monster.AddMonsterBagData(
			//            kData.m_iMonParentBagNo,
			//            kData.m_strMemo,
			//            kData.m_aiMonsterID,
			//            kData.m_aiMonsterRate);
			//    }
			//}
			//sr.Close();
		}

		private void m_btnBagSave_Click(object sender, EventArgs e)
		{
			SaveFileDialog dlg = new SaveFileDialog();

			string streamingDescriptions = "BAG files (*.bag)|*.bag";
			dlg.Filter = streamingDescriptions;

			if (dlg.ShowDialog() == System.Windows.Forms.DialogResult.OK)
			{
				string strFileName = dlg.FileName;
				SaveBagData(strFileName);
			}
		}

		private void m_btnBagLoad_Click(object sender, EventArgs e)
		{
			//OpenFileDialog dlg = new OpenFileDialog();
			//dlg.Filter = "BAG files (*.bag)|*.bag";
			//if (dlg.ShowDialog() == System.Windows.Forms.DialogResult.OK)
			//{
			//    m_lbMonsterBagList.Items.Clear();
			//    m_pmMonsterBagList.Clear();
			//    MFramework.Instance.Monster.ClearMonsterBagData();

			//    string strFileName = dlg.FileName;

			//    LoadBagData(strFileName);
			//}
		}

		private void m_btnApplyRegenPeriod_Click(object sender, EventArgs e)
		{
			int iPeriod = 0;
			bool bCheck = int.TryParse(m_tbRegenPeriod.Text, out iPeriod);
			if (!bCheck)
			{
				return;
			}

			MEntity[] amSelectedEntities = SelectionService
				.GetSelectedEntities();

			// MPgMonster Apply
			for (int i = 0; i < amSelectedEntities.Length; i++)
			{
				MEntity pkEntity = amSelectedEntities[i];
				MFramework.Instance.Monster.ModifyRegenPeriod(pkEntity, iPeriod);

				// C# Setted Monster Apply
				for (int j = 0; j < m_pmSettedMonsterList.Count; j++)
				{
					MonsterData kData = (MonsterData)m_pmSettedMonsterList[j];
					if (kData.m_pkEntity == pkEntity)
					{
						kData.m_iRegenPeriod = iPeriod;
					}
				}
			}
		}

		private void m_btnApplyPointGroup_Click(object sender, EventArgs e)
		{
			int iPointGroup = 0;
			if (!int.TryParse(m_tbPointGroup.Text, out iPointGroup))
			{
				return;
			}

			MEntity[] amSelectedEntities = SelectionService
				.GetSelectedEntities();

			// MPgMonster Apply
			for (int i = 0; i < amSelectedEntities.Length; i++)
			{
				MEntity pkEntity = amSelectedEntities[i];
				MFramework.Instance.Monster.ModifyPointGroup(pkEntity, iPointGroup);

				// C# Setted Monster Apply
				for (int j = 0; j < m_pmSettedMonsterList.Count; j++)
				{
					MonsterData kData = (MonsterData)m_pmSettedMonsterList[j];
					if (kData.m_pkEntity == pkEntity)
					{
						kData.m_iPointGroup = iPointGroup;
					}
				}
			}
		}

		private void m_btnApplyParentBagNo_Click(object sender, EventArgs e)
		{
			int iParentBagNo = 0;
			bool bCheck = int.TryParse(m_tbParentBagNo.Text, out iParentBagNo);
			if (!bCheck)
			{
				return;
			}

			MEntity[] amSelectedEntities = SelectionService
				.GetSelectedEntities();

			// MPgMonster Apply
			for (int i = 0; i < amSelectedEntities.Length; i++)
			{
				MEntity pkEntity = amSelectedEntities[i];
				MFramework.Instance.Monster.ModifyParentBagNo(pkEntity, iParentBagNo);

				// C# Setted Monster Apply
				for (int j = 0; j < m_pmSettedMonsterList.Count; j++)
				{
					MonsterData kData = (MonsterData)m_pmSettedMonsterList[j];
					if (kData.m_pkEntity == pkEntity)
					{
						kData.m_iMonParentBagNo = iParentBagNo;
					}
				}
			}
		}

		private void m_btnApplyMemo_Click(object sender, EventArgs e)
		{
			string strMemo = m_tbMemo.Text;

			MEntity[] amSelectedEntities = SelectionService
				.GetSelectedEntities();

			// MPgMonster Apply
			for (int i = 0; i < amSelectedEntities.Length; i++)
			{
				MEntity pkEntity = amSelectedEntities[i];
				MFramework.Instance.Monster.ModifyMemo(pkEntity, strMemo);

				// C# Setted Monster Apply
				for (int j = 0; j < m_pmSettedMonsterList.Count; j++)
				{
					MonsterData kData = (MonsterData)m_pmSettedMonsterList[j];
					if (kData.m_pkEntity == pkEntity)
					{
						kData.m_strMemo = strMemo;
					}
				}
			}
		}

		private void m_btnCreateMonster_Click(object sender, EventArgs e)
		{
            MPalette pkPalette = null;
            string strEntityName;
            if (true == m_cbAlteredMonsterShape.Checked)
            {
                pkPalette = MFramework.Instance.PaletteManager.GetPaletteByName("Default");

                strEntityName = "[Default]monster_target";
            }
            else
            {
                pkPalette = MFramework.Instance.PaletteManager.GetPaletteByName("Monster");
                
                string strMonsterID = GetMonsterIDFromBagNo(Int32.Parse(m_tbParentBagNo.Text));
                //if (null == strMonsterID)
                //{
                //    MessageBox.Show(m_tbParentBagNo.Text + "�� �˻��� ���Ͱ� �����ϴ�.");
                //    return;
                //}
                /*TODO: ASAP: Make constant bridge from C++ (MPgExtEntity.h)*/
                strEntityName = "[Monster]" + strMonsterID;
            }
			if (pkPalette == null)
			{
				MessageBox.Show("Monster or Default �ȷ�Ʈ�� �����ϴ�.");
				return;
			}
			//string strMonsterID = MFramework.Instance.Monster.GetMonsterTemplate(Int32.Parse(m_tbParentBagNo.Text));
            //string strMonsterID = GetMonsterIDFromBagNo(Int32.Parse(m_tbParentBagNo.Text));
            //if (null == strMonsterID)
            //{
            //    MessageBox.Show(m_tbParentBagNo.Text + "�� �˻��� ���Ͱ� �����ϴ�.");
            //    return;
            //}
			//GetMonsterIDFromBagNo(Int32.Parse(m_tbParentBagNo.Text));
            MEntity pkTemplate = pkPalette.GetEntityByName(strEntityName);
			if (pkTemplate == null)
			{
				//MessageBox.Show("[Monster]" + strMonsterID + " �� �����ϴ�.");
				//return;
				//pkTemplate = pkPalette.GetEntityByName("[Default]monster_target");
                ListViewItem kItem = m_lvLoadFailed.Items.Add(strEntityName);
				kItem.SubItems.Add(m_tbParentBagNo.Text);
			}

			MScene pmScene = MFramework.Instance.Scene;

			int iPointGroup = 0;
			int.TryParse(m_tbPointGroup.Text, out iPointGroup);
			int iPeriod = 0;
			int.TryParse(m_tbRegenPeriod.Text, out iPeriod);
			int iParentBagNo = 0;
			int.TryParse(m_tbParentBagNo.Text, out iParentBagNo);

			MonsterData kData = new MonsterData();
			kData.Initilaize(Guid.NewGuid(), m_tbMemo.Text, m_iMapNo, iParentBagNo, iPointGroup, iPeriod, 0, 0, 0, 0, 100);

			MFramework.Instance.PaletteManager.ActivePalette = pkPalette;
			if (MFramework.Instance.PaletteManager.ActivePalette.ActiveEntity !=
				pkTemplate)
			{
				MFramework.Instance.PaletteManager.ActivePalette.ActiveEntity = pkTemplate;
			}

			UICommand command = UICommandService.GetCommand(
				"CreateInteractionMode");
			if (command != null)
			{
				command.DoClick(this, null);
			}

			//ServiceProvider sp = ServiceProvider.Instance;
			//IEntityPathService pmPathService = sp.GetService(typeof(IEntityPathService)) as IEntityPathService;
            //string strMonName = "[Monster]";
			//strMonName = strMonName + m_tbMemo.Text;
			//string strCloneName = pmScene.GetUniqueEntityName(pmPathService.GetSimpleName(strMonName));
			//MEntity pkNewEntity = pkTemplate.Clone(strMonName, true);
			//kData.m_pkEntity = pkNewEntity;

			MFramework.Instance.Monster.StackSettedMonsterData(
				kData.m_strMemo, m_iMapNo, kData.m_iMonParentBagNo, kData.m_iPointGroup, kData.m_iRegenPeriod, kData.m_iMoveRange);
			//m_pmSettedMonsterList.Add(kData);
		}

		private void m_btnHideText_Click(object sender, EventArgs e)
		{
			MFramework.Instance.Monster.ChangeHideText();
		}

		private void m_cbGroupNum_SelectedIndexChanged(object sender, EventArgs e)
		{
            if (0==m_cbGroupNum.Text.CompareTo("All"))
            {
                MFramework.Instance.Monster.m_kGroupNum = MonsterBagControl.BAGTYPE.NONE;
            }
            else if (0==m_cbGroupNum.Text.CompareTo("Default"))
            {
                MFramework.Instance.Monster.m_kGroupNum = MonsterBagControl.BAGTYPE.DEFAULT;
            }
            else if (0 == m_cbGroupNum.Text.CompareTo("Level 1"))
            {
                MFramework.Instance.Monster.m_kGroupNum = MonsterBagControl.BAGTYPE.EASY;
            }
            else if (0 == m_cbGroupNum.Text.CompareTo("Level 2"))
            {
                MFramework.Instance.Monster.m_kGroupNum = MonsterBagControl.BAGTYPE.NORMAL;
            }
            else if (0 == m_cbGroupNum.Text.CompareTo("Level 3"))
            {
                MFramework.Instance.Monster.m_kGroupNum = MonsterBagControl.BAGTYPE.HARD;
            }
            else if (0 == m_cbGroupNum.Text.CompareTo("Level 4"))
            {
                MFramework.Instance.Monster.m_kGroupNum = MonsterBagControl.BAGTYPE.ULTRA;
            }
            else if (0 == m_cbGroupNum.Text.CompareTo("Level 5"))
            {
                MFramework.Instance.Monster.m_kGroupNum = MonsterBagControl.BAGTYPE.LEVEL05;
            }
            else if (0 == m_cbGroupNum.Text.CompareTo("Level 6"))
            {
                MFramework.Instance.Monster.m_kGroupNum = MonsterBagControl.BAGTYPE.LEVEL06;
            }
            MFramework.Instance.Monster.SetSettedMonsterListGroupVisible();
		}

		private void m_tbXmlPath_TextChanged(object sender, EventArgs e)
		{
			if (("" != m_tbXmlPath.Text) && ("" != m_tbMapNo.Text))
				m_btnLoad.Enabled = true;
			else
				m_btnLoad.Enabled = false;

			m_strXmlPath = m_tbXmlPath.Text;
		}

		private void m_tbMapNo_TextChanged(object sender, EventArgs e)
		{
			if (("" != m_tbXmlPath.Text) && ("" != m_tbMapNo.Text))
				m_btnLoad.Enabled = true;
			else
				m_btnLoad.Enabled = false;
		}

		private void m_btnXmlpath_Click(object sender, EventArgs e)
		{
			FolderBrowserDialog kFolderDlg = new FolderBrowserDialog();

			if (kFolderDlg.ShowDialog() == System.Windows.Forms.DialogResult.OK)
			{
				m_strXmlPath = kFolderDlg.SelectedPath;
				MFramework.Instance.Monster.m_strXmlPath = m_strXmlPath.ToString();
				m_tbXmlPath.Text = m_strXmlPath;

				LoadXml();
			}
		}

		private void LoadXml()
		{
			string strPath = m_tbXmlPath.Text + "\\" + "ClassNo.xml";
			m_kClassNoXmlManager.XmlFileLoad(strPath);
			m_kClassNoXmlDoc = m_kClassNoXmlManager.GetDocument();
			m_kClassNoXmlNodelist = m_kClassNoXmlDoc.GetElementsByTagName("ITEM");

			strPath = m_tbXmlPath.Text + "\\" + "path.xml";
			m_kPathXmlManager.XmlFileLoad(strPath);
			m_kPathXmlDoc = m_kPathXmlManager.GetDocument();
			m_kPathXmlNodelist = m_kPathXmlDoc.GetElementsByTagName("ACTOR");
		}

		private void button1_Click(object sender, EventArgs e)
		{
			//BuildMonsterTemplateTable();
		}

        private void m_btnApplyBagNum_Click(object sender, EventArgs e)
        {
            int iParentBagNo = 0;
            bool bCheck = int.TryParse(m_tbParentBagNo.Text, out iParentBagNo);
            if (!bCheck)
            {
                return;
            }

            if (false == m_pmMonsterBagControlList.ContainsKey(iParentBagNo))
            {
                MonsterBagControl kAdd = new MonsterBagControl();
                kAdd.m_iParentBagNo = iParentBagNo;
                kAdd.m_IsEditing = true;
                m_pmMonsterBagControlList.Add(kAdd.m_iParentBagNo, kAdd);
            }

            MonsterBagControl kData = (MonsterBagControl)m_pmMonsterBagControlList[iParentBagNo];

            int iBagNo = 0;
            if (int.TryParse(m_lvBagControl.Items[0].SubItems[MONSTERBAGLISTVIEW_EDITBOX].Text, out iBagNo))
            {
                kData.m_iDefaultBagNo = m_lvBagControl.Items[0].Checked ? iBagNo : 0;
            }
            if (int.TryParse(m_lvBagControl.Items[1].SubItems[MONSTERBAGLISTVIEW_EDITBOX].Text, out iBagNo))
            {
                kData.m_iEasyBagNo = m_lvBagControl.Items[1].Checked ? iBagNo : 0;
            }
            if (int.TryParse(m_lvBagControl.Items[2].SubItems[MONSTERBAGLISTVIEW_EDITBOX].Text, out iBagNo))
            {
                kData.m_iNormalBagNo = m_lvBagControl.Items[2].Checked ? iBagNo : 0;
            }
            if (int.TryParse(m_lvBagControl.Items[3].SubItems[MONSTERBAGLISTVIEW_EDITBOX].Text, out iBagNo))
            {
                kData.m_iHardBagNo = m_lvBagControl.Items[3].Checked ? iBagNo : 0;
            }
            if (int.TryParse(m_lvBagControl.Items[4].SubItems[MONSTERBAGLISTVIEW_EDITBOX].Text, out iBagNo))
            {
                kData.m_iUltraBagNo = m_lvBagControl.Items[4].Checked ? iBagNo : 0;
            }
            if (int.TryParse(m_lvBagControl.Items[5].SubItems[MONSTERBAGLISTVIEW_EDITBOX].Text, out iBagNo))
            {
                kData.m_iLevel05Bag = m_lvBagControl.Items[5].Checked ? iBagNo : 0;
            }
            if (int.TryParse(m_lvBagControl.Items[6].SubItems[MONSTERBAGLISTVIEW_EDITBOX].Text, out iBagNo))
            {
                kData.m_iLevel06Bag = m_lvBagControl.Items[6].Checked ? iBagNo : 0;
            }
            kData.m_IsEditing = true;
        }

        private void m_tbParentBagNo_TextChanged(object sender, EventArgs e)
        {
            TextBox kTextBox = (TextBox)sender;
            m_tbParentBagNo.Text = kTextBox.Text;

            int iParentBagNo = 0;
            int.TryParse(m_tbParentBagNo.Text, out iParentBagNo);
            UpdateMonsterBagListView(iParentBagNo);
        }

        private void m_cbAlteredMonsterShape_CheckedChanged(object sender, EventArgs e)
        {
            MFramework.Instance.PaletteManager.SetAlteredMonsterShape(m_cbAlteredMonsterShape.Checked);
        }

        private void MonsterPanel_Load(object sender, EventArgs e)
        {
            MFramework.Instance.PaletteManager.SetAlteredMonsterShape(m_cbAlteredMonsterShape.Checked);
        }
	}

	public class MonsterData : Object
	{
		#region Private Data
		public Guid m_kGuid = Guid.Empty;
		public string m_strMemo = null;
		public int m_iMapNo = 0;
		public int m_iMonParentBagNo = 0;
		public int m_iPointGroup = 0;
		public int m_iRegenPeriod = 0;
		public int m_iMoveRange = 100;
		public float m_fPosX = 0.0f;
		public float m_fPosY = 0.0f;
		public float m_fPosZ = 0.0f;
		public float m_fRotAxZ = 0.0f;
		public MEntity m_pkEntity;
        public MonsterData m_kInitData = null;

		// Monster Bag
		public int[] m_aiMonsterID = new int[10];
		public int[] m_aiMonsterRate = new int[10];
		#endregion

		public void Initilaize(Guid kGuid, string strMemo, int iMapNo, int iMonParentBagNo,
			int iPointGroup, int iRegenPeriod, float fPosX, float fPosY, float fPosZ, float fRot, int iMoveRange)
		{
            InitilaizeNoBackup(kGuid, strMemo, iMapNo, iMonParentBagNo, iPointGroup, iRegenPeriod, fPosX, fPosY, fPosZ, fRot, iMoveRange);
            m_kInitData = new MonsterData();
            m_kInitData.InitilaizeNoBackup(kGuid, strMemo, iMapNo, iMonParentBagNo, iPointGroup, iRegenPeriod, fPosX, fPosY, fPosZ, fRot, iMoveRange);
		}

        public void InitilaizeNoBackup(Guid kGuid, string strMemo, int iMapNo, int iMonParentBagNo,
            int iPointGroup, int iRegenPeriod, float fPosX, float fPosY, float fPosZ, float fRot, int iMoveRange)
        {
            m_kGuid = kGuid;
            m_strMemo = strMemo;
            m_iMapNo = iMapNo;
            m_iPointGroup = iPointGroup;
            m_iMonParentBagNo = iMonParentBagNo;
            m_iPointGroup = iPointGroup;
            m_iRegenPeriod = iRegenPeriod;
            m_fPosX = fPosX;
            m_fPosY = fPosY;
            m_fPosZ = fPosZ;
            m_fRotAxZ = fRot;
            m_iMoveRange = iMoveRange;
        }

        public bool IsChange()
        {
            if (null == m_kInitData)
            {
                return false;
            }

            return (this.m_kGuid != m_kInitData.m_kGuid ||
                    this.m_strMemo != m_kInitData.m_strMemo ||
                    this.m_iMapNo != m_kInitData.m_iMapNo ||
                    this.m_iPointGroup != m_kInitData.m_iPointGroup ||
                    this.m_iMonParentBagNo != m_kInitData.m_iMonParentBagNo ||
                    this.m_iPointGroup != m_kInitData.m_iPointGroup ||
                    this.m_iRegenPeriod != m_kInitData.m_iRegenPeriod ||
                    this.m_fPosX != m_kInitData.m_fPosX ||
                    this.m_fPosY != m_kInitData.m_fPosY ||
                    this.m_fPosZ != m_kInitData.m_fPosZ ||
                    this.m_fRotAxZ != m_kInitData.m_fRotAxZ ||
                    this.m_iMoveRange != m_kInitData.m_iMoveRange);

        }
	}
}