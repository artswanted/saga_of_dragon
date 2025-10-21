using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace Emergent.Gamebryo.SceneDesigner.GUI.ProjectG
{
	public partial class RainDataIgnoreListDialog : Form
	{
		public string m_strNodename;

		public RainDataIgnoreListDialog()
		{
			InitializeComponent();
		}
		
		public void Initialize(string strNodename)
		{
			if (strNodename != null)
			{
				m_strNodename = strNodename;
				m_tbNodename.Text = strNodename;
			}
		}

		private void m_btnOK_Click(object sender, EventArgs e)
		{
			if (m_tbNodename.Text.Equals("") ||
				m_tbNodename.Text == null)
			{
				MessageBox.Show("내용을 입력하세요.", "error");
				return;
			}

			m_strNodename = m_tbNodename.Text;
			this.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.Close();
		}
	}
}