using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace AnimationToolUI.Forms
{
    public partial class CloneSequenceForm : Form
    {
        String m_kNewSequenceName;

        public CloneSequenceForm()
        {
            InitializeComponent();
        }

        private void OnCancel(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
        }

        private void OnOK(object sender, EventArgs e)
        {
            m_kNewSequenceName = tbName.Text;

            DialogResult = DialogResult.OK;
        }

        public String GetNewSequenceName()
        {
            return m_kNewSequenceName;
        }
    }
}