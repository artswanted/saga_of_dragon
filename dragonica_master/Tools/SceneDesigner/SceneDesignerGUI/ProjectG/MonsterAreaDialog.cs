using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace Emergent.Gamebryo.SceneDesigner.GUI.ProjectG
{
	public partial class MonsterAreaDialog : Form
	{
		//public String m_strGroupName;
		public String m_strLinkArea;
		public int m_iResult;

		public MonsterAreaDialog()
		{
			InitializeComponent();
		}

		private void m_btnOk_Click(object sender, EventArgs e)
		{
			m_strLinkArea = m_tbGroupName.Text;
			int iResult;
			if (Int32.TryParse(m_strLinkArea, out iResult))
			{
				m_iResult = iResult;
				this.DialogResult = System.Windows.Forms.DialogResult.OK;
				this.Close();
			}
			else
			{
				MessageBox.Show("숫자만 입력하세요.");
			}
		}

		public void Update()
		{
			m_strLinkArea = m_iResult.ToString();
			m_tbGroupName.Text = m_strLinkArea;
		}
	}
}