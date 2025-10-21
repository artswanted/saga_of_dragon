using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace Emergent.Gamebryo.SceneDesigner.GUI.ProjectG
{
	public partial class MonsterPanelSetting : Form
	{
		public string m_strServer;
		public string m_strID;
		public string m_strPassword;
		public string m_strDatabase;
		public string m_strMonsterTable;
		public string m_strRegenTable;
        public string m_strBagControl;
        public string m_strObjectBag;
        public string m_strObjectElement;

		public MonsterPanelSetting()
		{
			InitializeComponent();
		}

		public void Initialize()
		{
			m_tbServer.Text = m_strServer;
			m_tbID.Text = m_strID;
			m_tbPassword.Text = m_strPassword;
			m_tbDatabase.Text = m_strDatabase;
			m_tbMonsterTable.Text = m_strMonsterTable;
			m_tbRegenTable.Text = m_strRegenTable;
            m_tbBagControl.Text = m_strBagControl;
            m_tbObjectBag.Text = m_strObjectBag;
            m_tbObjectElement.Text = m_strObjectElement;
		}

		private void m_btnOK_Click(object sender, EventArgs e)
		{
			m_strServer = m_tbServer.Text;
			m_strID = m_tbID.Text;
			m_strPassword = m_tbPassword.Text;
			m_strDatabase = m_tbDatabase.Text;
			m_strMonsterTable = m_tbMonsterTable.Text;
			m_strRegenTable = m_tbRegenTable.Text;
            m_strBagControl = m_tbBagControl.Text;
            m_strObjectBag = m_tbObjectBag.Text;
            m_strObjectElement = m_tbObjectElement.Text;
			this.DialogResult = DialogResult.OK;
			this.Close();
        }

		private void m_btnCancel_Click(object sender, EventArgs e)
		{
			this.DialogResult = DialogResult.No;
			this.Close();
		}
	}
}