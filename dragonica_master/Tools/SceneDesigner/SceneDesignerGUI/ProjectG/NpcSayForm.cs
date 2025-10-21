using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace Emergent.Gamebryo.SceneDesigner.GUI.ProjectG
{
	public partial class NpcSayForm : Form
	{
		public string m_strTYPE = "IDLE";
		public string m_strTTW = "83";
		public string m_strUPTIME = "6.3";
		public string m_strDELAY = "1.0";
		public string m_strACTION = "a_idle";

		public NpcSayForm()
		{
			InitializeComponent();
			m_tbTYPE.Text = m_strTYPE;
			m_tbTTW.Text = m_strTTW;
			m_tbUPTIME.Text = m_strUPTIME;
			m_tbDELAY.Text = m_strDELAY;
			m_tbACTION.Text = m_strACTION;
		}

		public void SetData(string strTYPE, string strTTW, string strUPTIME, string strDELAY, string strACTION)
		{
			m_strTYPE = strTYPE;
			m_strTTW = strTTW;
			m_strUPTIME = strUPTIME;
			m_strDELAY = strDELAY;
			m_strACTION = strACTION;

			m_tbTYPE.Text = m_strTYPE;
			m_tbTTW.Text = m_strTTW;
			m_tbUPTIME.Text = m_strUPTIME;
			m_tbDELAY.Text = m_strDELAY;
			m_tbACTION.Text = m_strACTION;
		}

		private void m_btnOK_Click(object sender, EventArgs e)
		{
			m_strTYPE = m_tbTYPE.Text;
			m_strTTW = m_tbTTW.Text;
			m_strUPTIME = m_tbUPTIME.Text;
			m_strDELAY = m_tbDELAY.Text;
			m_strACTION = m_tbACTION.Text;

			this.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.Close();
		}

		private void m_btnCancel_Click(object sender, EventArgs e)
		{
			this.Close();
		}
	}
}