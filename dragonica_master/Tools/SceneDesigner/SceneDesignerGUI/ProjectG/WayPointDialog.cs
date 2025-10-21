using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace Emergent.Gamebryo.SceneDesigner.GUI.ProjectG
{
	public partial class WayPointDialog : Form
	{
		public String m_strGroupName;

		public WayPointDialog()
		{
			InitializeComponent();
		}

		private void m_btnOk_Click(object sender, EventArgs e)
		{
			m_strGroupName = m_tbGroupName.Text;

			this.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.Close();
		}

		public void Update()
		{
			m_tbGroupName.Text = m_strGroupName;
		}
	}
}