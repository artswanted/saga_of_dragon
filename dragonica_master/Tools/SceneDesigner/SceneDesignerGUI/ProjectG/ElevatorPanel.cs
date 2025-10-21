using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;
using System.Collections;
using System.Threading;
using System.Xml;
using System.IO;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.GUI.ProjectG;

namespace Emergent.Gamebryo.SceneDesigner.GUI.ProjectG
{
    public partial class ElevatorPanel : Form
    {
        #region Private Data
        private IUICommandService m_uiCommandService;
        private string m_strPosX;
        private string m_strPosY;
        private string m_strPosZ;
        private string m_strMapNum;

        private XmlManager m_XmlManager;
        #endregion
        public ElevatorPanel()
        {
            InitializeComponent();

            UICommandService.BindCommands(this);

            cbRidable.SelectedIndex = 1;
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

        #region Service Accessors
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
        #endregion

        public void SetXmlManager(ref XmlManager pXmlManager)
        {
            m_XmlManager = pXmlManager;
        }

        public void InitPanelListLoad()
        {
            //m_lbNpcSets.Items.Clear();

            XmlDocument pDoc = m_XmlManager.GetDocument();
            XmlNodeList nodeList = pDoc.GetElementsByTagName("OBJECT");

			MFramework.Instance.Elevator.SetXmlDocument(m_XmlManager.GetDocument());

            MPalette pkPalette = MFramework.Instance.PaletteManager.GetPaletteByName("General");
            if (pkPalette == null)
            {
                MessageBox.Show("General 팔레트가 없습니다.");
                return;
            }
            MEntity pkTemplate = pkPalette.GetEntityByName("[General]Target.elevator_target");
            if (pkTemplate == null)
            {
                MessageBox.Show("[General]Target.elevator_target 가 없습니다.");
                return;
            }

            for (int ii = 0; ii < nodeList.Count; ++ii)
            {
                XmlNode kNode = nodeList.Item(ii);
				if (kNode.ChildNodes.Count > 0)
				{
					string strGuid = kNode.Attributes.GetNamedItem("GUID").InnerText;
					string strRidable = kNode.Attributes.GetNamedItem("RIDABLE").InnerText;
					string strSpeed = kNode.Attributes.GetNamedItem("SPEED").InnerText;
					string strClassNo = kNode.Attributes.GetNamedItem("CLASS_NO").InnerText;
					float fSpeed = 0;
					float.TryParse(strSpeed, out fSpeed);
					Guid kGuid = new Guid(strGuid);
					MFramework.Instance.Elevator.CreateElevator(strClassNo, strRidable, fSpeed, kGuid);

					for (int jj = 0; jj < kNode.ChildNodes.Count; ++jj)
					{
						XmlNode kChild = kNode.ChildNodes.Item(jj);
						string strPoint = kChild.Attributes.GetNamedItem("POINT").InnerText;
						string strTime = kChild.Attributes.GetNamedItem("TIME").InnerText;
						XML_LOCATION kLoc = new XML_LOCATION();
						kLoc = kLoc.StringsToLocation(strPoint);
						float fTime = 0;
						float fX = kLoc.fLocX;
						float fY = kLoc.fLocY;
						float fZ = kLoc.fLocZ;
						float.TryParse(strTime, out fTime);
						MFramework.Instance.Elevator.CreatePoint(ii, jj, fTime, fX, fY, fZ, null);
					}
				}
				else
				{
				}
            }

            SyncElevatorList();
            SyncPointList();
        }

		public void RearrangeList()
		{//
			int iIndex = 0;
			while (MFramework.Instance.Elevator.m_pmElevatorList.Count > iIndex)
			{
				SElevatorList kEList = (SElevatorList)MFramework.Instance.Elevator.m_pmElevatorList[iIndex];
				if (0 == kEList.m_pmPointList.Count)
				{
					MFramework.Instance.Elevator.m_pmElevatorList.Remove(kEList);
				}
				else
				{
					++iIndex;
				}
			}
			/*
			for (int iClearIndex = 0; MFramework.Instance.Elevator.m_pmElevatorList.Count > iClearIndex; ++iClearIndex)
			{
				SElevatorList kEList = (SElevatorList)MFramework.Instance.Elevator.m_pmElevatorList[iClearIndex];
				if (0 == kEList.m_pmPointList.Count)
				{
					MFramework.Instance.Elevator.m_pmElevatorList.Remove(kEList);
					if (0 < iClearIndex)
					{
						--iClearIndex;
					}
				}
			}*/
			
			XmlElement pRoot = m_XmlManager.GetDocument().DocumentElement;
			XmlDocument pDoc = m_XmlManager.GetDocument();
			XmlNodeList nodeList = pDoc.GetElementsByTagName("OBJECT");
			iIndex = 0;
			while (nodeList.Count > iIndex)
			{
				XmlNode kNode = nodeList.Item(iIndex);
				XmlElement kElement = (XmlElement)kNode;
				if (0 == kElement.ChildNodes.Count)
				{
					pRoot.RemoveChild(kNode);
				}
				else
				{
					++iIndex;
				}
			}
			/*
			for (int ii = 0; ii < nodeList.Count; ++ii)
			{
				XmlNode kNode = nodeList.Item(ii);
				XmlElement kElement = (XmlElement)kNode;

				if (0 == kElement.ChildNodes.Count)
				{
					pRoot.RemoveChild(kNode);
					if (0 < ii)
					{//노드를 하나 지우면 뒤의 노드들이 전부 앞으로 당겨지므로 ii를 감소시켜 당겨진 노드부터 다시 검사하도록 한다.
						--ii;
					}
				}
			}*/

			SyncElevatorList();
		}

		public void AlertAbnormalList()
		{
			string strAbnormalList = "";
			int iClearedElevatorCount = MFramework.Instance.Elevator.m_pmElevatorList.Count;
			for (int iClearIndex = 0; iClearedElevatorCount > iClearIndex; ++iClearIndex)
			{
				SElevatorList kEList = (SElevatorList)MFramework.Instance.Elevator.m_pmElevatorList[iClearIndex];
				if (1 == kEList.m_pmPointList.Count)
				{
					//MFramework.Instance.Elevator.m_pmElevatorList.Remove(kEList);
					strAbnormalList += kEList.m_kGuid + "\n";
				}
			}
			if ("" != strAbnormalList)
			{
				MessageBox.Show(strAbnormalList + "포인트를 한개만 가지고 있습니다.");
			}
		}

        private void tbPosX_TextChanged(object sender, EventArgs e)
        {
            m_strPosX = tbPosX.Text;
        }

        private void tbPosY_TextChanged(object sender, EventArgs e)
        {
            m_strPosY = tbPosY.Text;
        }

        private void tbPosZ_TextChanged(object sender, EventArgs e)
        {
            m_strPosZ = tbPosZ.Text;
        }

        private void tbMapNum_TextChanged(object sender, EventArgs e)
        {
            m_strMapNum = tbMapNum.Text;
            MFramework.Instance.Elevator.m_strFileName = m_strMapNum;
        }

        private void btnLoad_Click(object sender, EventArgs e)
        {
            //OpenFileDialog dlg = new OpenFileDialog();
            //dlg.Filter = "XML files (*.xml) |*.xml";
            //dlg.CheckFileExists = true;
            //dlg.Multiselect = false;
            //if (dlg.ShowDialog() == DialogResult.OK)
            //{
            //    m_strMapNum = dlg.FileName;
            //    tbMapNum.Text = m_strMapNum;
            //    MFramework.Instance.Elevator.m_strFileName = m_strMapNum;
            //    MFramework.Instance.Elevator.LoadXML();
            //}
        }

        private void btnSave_Click(object sender, EventArgs e)
        {
            //MFramework.Instance.Elevator.SaveXML();
        }

        private void lbElevator_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (lbElevator.SelectedIndex == -1)
                return;

            if (MFramework.Instance.Elevator.m_pmElevatorList.Count <= lbElevator.SelectedIndex)
                return;

            SElevatorList kElevator = (SElevatorList)MFramework.Instance.Elevator.m_pmElevatorList[lbElevator.SelectedIndex];

            tbID.Text = kElevator.m_strClassNo;
            tbSpeed.Text = kElevator.m_fSpeed.ToString();
            cbRidable.Text = kElevator.m_strRidable.ToString();

            SyncPointList();
        }

        private void lbPoints_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (lbElevator.SelectedIndex == -1 || lbPoints.SelectedIndex == -1)
                return;

            if (MFramework.Instance.Elevator.m_pmElevatorList.Count <= lbElevator.SelectedIndex)
                return;

            SElevatorList kElevator = (SElevatorList)MFramework.Instance.Elevator.m_pmElevatorList[lbElevator.SelectedIndex];
            if (kElevator.m_pmPointList.Count <= lbPoints.SelectedIndex)
                return;

            SPointList kPoint = (SPointList)kElevator.m_pmPointList[lbPoints.SelectedIndex];

            tbTime.Text = kPoint.m_fTime.ToString();
        }

        private void btnAdd_Click(object sender, EventArgs e)
        {
            MPalette pkPalette = MFramework.Instance.PaletteManager.GetPaletteByName("General");
            if (pkPalette == null)
            {
                MessageBox.Show("General 팔레트가 없습니다.");
                return;
            }
            MEntity pkTemplate = pkPalette.GetEntityByName("[General]Target.elevator_target");
            if (pkTemplate == null)
            {
                MessageBox.Show("[General]Target.elevator_target 가 없습니다.");
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
                if (lbElevator.SelectedIndex == -1)
                {
                    MessageBox.Show("SelectElevator Failed");
                    return;
                }
                //int iPointNum;
                float fPointTime;

                bool bCheck = float.TryParse(tbTime.Text, out fPointTime);
                if (!bCheck)
                {
                    MessageBox.Show("PointTime Failed");
                    return;
                }
                MFramework.Instance.Elevator.m_iSelectElevatorList = lbElevator.SelectedIndex;
                MFramework.Instance.Elevator.m_fPointTime = fPointTime;

                command.DoClick(this, null);
            }
        }

        private void btnCreate_Click(object sender, EventArgs e)
        {
            if (tbID == null || tbSpeed == null)
            {
                MessageBox.Show("Insert ID and Speed");
                return;
            }
            float fSpeed;
            bool bCheck = float.TryParse(tbSpeed.Text, out fSpeed);
            if (!bCheck)
            {
                MessageBox.Show("Speed Failed");
                return;
            }

            Guid kGuid = MFramework.Instance.Elevator.CreateElevator(tbID.Text, cbRidable.Text, fSpeed, Guid.Empty);

            XmlDocument pDoc = m_XmlManager.GetDocument();
            XmlElement kElement = pDoc.CreateElement("OBJECT");
            kElement.SetAttribute("GUID", kGuid.ToString());
            kElement.SetAttribute("RIDABLE", cbRidable.Text);
            kElement.SetAttribute("SPEED", tbSpeed.Text);
            kElement.SetAttribute("CLASS_NO", tbID.Text);

            XmlElement pRoot = m_XmlManager.GetDocument().DocumentElement;
            pRoot.AppendChild(kElement);
            SyncElevatorList();
        }

        private void btnElevatorDelete_Click(object sender, EventArgs e)
        {
            if (lbElevator.SelectedIndex == -1)
                return;

            SElevatorList kElevator = (SElevatorList)MFramework.Instance.Elevator.m_pmElevatorList[lbElevator.SelectedIndex];
            //string strClassNo = kElevator.m_strClassNo;
            string strGuid = kElevator.m_kGuid.ToString();

            MFramework.Instance.Elevator.DeleteElevator(lbElevator.SelectedIndex);

            XmlElement pRoot = m_XmlManager.GetDocument().DocumentElement;
            XmlDocument pDoc = m_XmlManager.GetDocument();
            XmlNodeList nodeList = pDoc.GetElementsByTagName("OBJECT");
            for (int ii = 0; ii < nodeList.Count; ++ii)
            {
                XmlNode kNode = nodeList.Item(ii);
                XmlElement kElement = (XmlElement)kNode;
                string strNodeID = kElement.GetAttribute("GUID");
                if(strGuid.Equals(strNodeID))
                {
                    pRoot.RemoveChild(kNode);
                    break;
                }
                //string strNodeID = kElement.GetAttribute("CLASS_NO");
                //if (strNodeID.Equals(strClassNo))
                //{
                //    pRoot.RemoveChild(kNode);
                //    break;
                //}
            }

            SyncElevatorList();
        }

        private void SyncElevatorList()
        {
            lbElevator.Items.Clear();
            int nCount = MFramework.Instance.Elevator.m_pmElevatorList.Count;
            if (nCount == 0)
                return;

            for (int ii = 0; ii < nCount; ++ii)
            {
                SElevatorList kElevator = (SElevatorList)MFramework.Instance.Elevator.m_pmElevatorList[ii];
                string strText = "ClassNo:" + kElevator.m_strClassNo + ", SPEED:" + kElevator.m_fSpeed + ", Guid:" + kElevator.m_kGuid;
                lbElevator.Items.Add(strText);
            }
        }

        private void SyncPointList()
        {
			SelectionService.ClearSelectedEntities();
            lbPoints.Items.Clear();
            if (lbElevator.SelectedIndex == -1)
                return;

            if (MFramework.Instance.Elevator.m_pmElevatorList.Count <= lbElevator.SelectedIndex)
                return;

            SElevatorList kElevator = (SElevatorList)MFramework.Instance.Elevator.m_pmElevatorList[lbElevator.SelectedIndex];

            for (int ii = 0; ii < kElevator.m_pmPointList.Count; ++ii)
            {
                SPointList kPoint = (SPointList)kElevator.m_pmPointList[ii];
                string strText = "Pos:" + kPoint.m_fPosX + ", " + kPoint.m_fPosY + ", "
                    + kPoint.m_fPosZ + ", Time:" + kPoint.m_fTime;
                lbPoints.Items.Add(strText);
				SelectionService.AddEntityToSelection(kPoint.m_pmEntity);
            }
        }

        private void btnPointRemove_Click(object sender, EventArgs e)
        {
            if (lbElevator.SelectedIndex == -1 || lbPoints.SelectedIndex == -1)
                return;

            SElevatorList kElevator = (SElevatorList)MFramework.Instance.Elevator.m_pmElevatorList[lbElevator.SelectedIndex];
            string strClassNo = kElevator.m_strClassNo;
            SPointList kPoint = (SPointList)kElevator.m_pmPointList[lbPoints.SelectedIndex];

            MFramework.Instance.Elevator.DeletePoint(lbElevator.SelectedIndex, lbPoints.SelectedIndex);

            XmlDocument pDoc = m_XmlManager.GetDocument();
            XmlNodeList nodeList = pDoc.GetElementsByTagName("OBJECT");
            XmlNode kNode = nodeList.Item(lbElevator.SelectedIndex);
            XmlNode kChildNode = kNode.ChildNodes.Item(lbPoints.SelectedIndex);
            kNode.RemoveChild(kChildNode);

            SyncPointList();
        }

        private void btnModify_Click(object sender, EventArgs e)
        {
            if (lbElevator.SelectedIndex == -1 || lbPoints.SelectedIndex == -1)
                return;

            float fTime;
            bool bCheck = float.TryParse(tbTime.Text, out fTime);
            if (!bCheck)
                return;

            SElevatorList kElevator = (SElevatorList)MFramework.Instance.Elevator.m_pmElevatorList[lbElevator.SelectedIndex];
            SPointList kPoint = (SPointList)kElevator.m_pmPointList[lbPoints.SelectedIndex];

            MFramework.Instance.Elevator.ModifyPoint(lbElevator.SelectedIndex, lbPoints.SelectedIndex, fTime);

            XmlDocument pDoc = m_XmlManager.GetDocument();
            XmlNodeList nodeList = pDoc.GetElementsByTagName("OBJECT");

            XmlNode kNode = nodeList.Item(lbElevator.SelectedIndex);
            XmlNode kChildNode = kNode.ChildNodes.Item(lbPoints.SelectedIndex);
            XmlElement kChildElement = (XmlElement)kChildNode;
            kChildElement.SetAttribute("TIME", fTime.ToString());

            SyncPointList();
        }

        private void btnElevatorModify_Click(object sender, EventArgs e)
        {
            if (lbElevator.SelectedIndex == -1)
                return;

            float fSpeed;
            bool bCheck = float.TryParse(tbSpeed.Text, out fSpeed);
            if (!bCheck)
                return;

            SElevatorList kElevator = (SElevatorList)MFramework.Instance.Elevator.m_pmElevatorList[lbElevator.SelectedIndex];
            string strClassNo = kElevator.m_strClassNo;

            MFramework.Instance.Elevator.ModifyElevator(lbElevator.SelectedIndex, tbID.Text, cbRidable.Text, fSpeed);

            XmlDocument pDoc = m_XmlManager.GetDocument();
            XmlNodeList nodeList = pDoc.GetElementsByTagName("OBJECT");

            XmlNode kNode = nodeList.Item(lbElevator.SelectedIndex);
            XmlElement kElement = (XmlElement)kNode;
            
            kElement.SetAttribute("CLASS_NO", tbID.Text);
            kElement.SetAttribute("RIDABLE", cbRidable.Text);
            kElement.SetAttribute("SPEED", tbSpeed.Text);

            SyncElevatorList();
        }

        private void lbPoints_DoubleClick(object sender, System.EventArgs e)
        {
            if (MFramework.Instance.CameraManager.IsSceneCamera())//!/
                return;

            if (lbElevator.SelectedIndex == -1 || lbPoints.SelectedIndex == -1)
                return;

            SElevatorList kElevator = (SElevatorList)MFramework.Instance.Elevator.m_pmElevatorList[lbElevator.SelectedIndex];
            SPointList kPoint = (SPointList)kElevator.m_pmPointList[lbPoints.SelectedIndex];

            MEntity pkEntity = kPoint.m_pmEntity;

            SelectionService.ClearSelectedEntities();
            SelectionService.AddEntityToSelection(pkEntity);

            ServiceProvider sp = ServiceProvider.Instance;
            IUICommandService uiCommandService = sp.GetService(typeof(IUICommandService)) as IUICommandService;

            UICommand zoomCommand = uiCommandService.GetCommand("MoveToSelection");
            UIState state = new UIState();
            zoomCommand.ValidateCommand(state);
            if (state.Enabled)
            {
                zoomCommand.DoClick(this, null);
            }
        }

		public void Clear()
		{
			lbElevator.Items.Clear();
			lbPoints.Items.Clear();
			MFramework.Instance.Elevator.Clear();
		}

        #region WinForms event handlers
        [UICommandHandler("Idle")]
        private void Application_Idle(object sender, EventArgs e)
        {
            if (MFramework.Instance.Elevator.GetSyncData())
            {
                SyncElevatorList();
                SyncPointList();
                MFramework.Instance.Elevator.SetSyncData(false);
            }

            if (MFramework.Instance.Elevator.m_bAdd)
            {
                int iElevator = MFramework.Instance.Elevator.m_iAddElevatorIndex;
                int iPoint = MFramework.Instance.Elevator.m_iAddPointIndex;

                SElevatorList kElevator = (SElevatorList)MFramework.Instance.Elevator.m_pmElevatorList[iElevator];
                SPointList kPoint = (SPointList)kElevator.m_pmPointList[iPoint];

                XmlDocument pDoc = m_XmlManager.GetDocument();
                XmlNodeList nodeList = pDoc.GetElementsByTagName("OBJECT");
                XmlNode kNode = nodeList.Item(iElevator);

                XmlElement kElemnent = pDoc.CreateElement("WayPoint");
                string strLoc = kPoint.m_fPosX + ", " + kPoint.m_fPosY + ", " + kPoint.m_fPosZ;
                kElemnent.SetAttribute("POINT", strLoc);
                kElemnent.SetAttribute("TIME", kPoint.m_fTime.ToString());
                kNode.AppendChild(kElemnent);

                MFramework.Instance.Elevator.m_bAdd = false;
            }

            if (true == MFramework.Instance.Elevator.m_bDelete)
            {
				//int iElevator = MFramework.Instance.Elevator.m_iDeleteElevatorIndex;
				//int iPoint = MFramework.Instance.Elevator.m_iDeletePointIndex;
				/*
				XmlDocument pDoc = m_XmlManager.GetDocument();
				XmlNodeList nodeList = pDoc.GetElementsByTagName("OBJECT");
				XmlNode kNode = nodeList.Item(iElevator);
				XmlNode kChild = kNode.ChildNodes.Item(iPoint);
				kNode.RemoveChild(kChild);
				*/
				MFramework.Instance.Elevator.m_bDelete = false;
				//MFramework.Instance.Elevator.RearrangeElevatorList();

				/*
				for (int iIndex = 0; MFramework.Instance.Elevator.m_pmDeleteList.Count > iIndex; ++iIndex)
				{
				    SDeleteList kDeleteObject = (SDeleteList)MFramework.Instance.Elevator.m_pmDeleteList[iIndex];
					//MFramework.Instance.Elevator.m_pmDeleteList[iIndex].
					
					XmlDocument pDoc = m_XmlManager.GetDocument();
					XmlNodeList nodeList = pDoc.GetElementsByTagName("OBJECT");
					XmlNode kNode = nodeList.Item(kDeleteObject.GetElevatorIndex());//iElevator);
					XmlNode kChild = kNode.ChildNodes.Item(kDeleteObject.GetPointIndex());//iPoint);
					if (null != kChild)
					{
						kNode.RemoveChild(kChild);
						if (0 == kNode.ChildNodes.Count)
						{

						}
					}
				}
				MFramework.Instance.Elevator.m_pmDeleteList.Clear();
                //MFramework.Instance.Elevator.m_bDelete = false;
				*/
				RearrangeList();
				//SyncElevatorList();
            }

            if (MFramework.Instance.Elevator.m_bTranslate)
            {
                int iCount = MFramework.Instance.Elevator.m_pmTranslateList.Count;
                for(int i=0; i<iCount; ++i)
                {
                    STranslateData kTranslate = (STranslateData)MFramework.Instance.Elevator.m_pmTranslateList[i];
                    int iElevator = kTranslate.m_iTranslateElevatorIndex;
                    int iPoint = kTranslate.m_iTranslatePointIndex;

                    SElevatorList kElevator = (SElevatorList)MFramework.Instance.Elevator.m_pmElevatorList[iElevator];
                    SPointList kPoint = (SPointList)kElevator.m_pmPointList[iPoint];

                    XmlDocument pDoc = m_XmlManager.GetDocument();
                    XmlNodeList nodeList = pDoc.GetElementsByTagName("OBJECT");
                    XmlNode kNode = nodeList.Item(iElevator);

                    XmlNode kChild = kNode.ChildNodes.Item(iPoint);
                    XmlElement kElement = (XmlElement)kChild;
                    string strLoc = kPoint.m_fPosX + ", " + kPoint.m_fPosY + ", " + kPoint.m_fPosZ;
                    kElement.SetAttribute("POINT", strLoc);
                }

                MFramework.Instance.Elevator.m_pmTranslateList.Clear();
                MFramework.Instance.Elevator.m_bTranslate = false;
            }
        #endregion

        }
    }
}