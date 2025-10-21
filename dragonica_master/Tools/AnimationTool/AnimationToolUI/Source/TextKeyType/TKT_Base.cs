using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using NiManagedToolInterface;

namespace AnimationToolUI.Source.TextKeyType
{
    public  class TKT_Base
    {
        protected TextKeyForm m_kForm;
        protected Control m_kGrpBase, m_kBtnAdd, m_kBtnCancel;
        protected Control m_kCmbType, m_kEbTextKey, m_kEbTime;

        public virtual bool Initialize(TextKeyForm kForm)
        {
            m_kForm = kForm;

            LinkControls();

            return true;
        }
        protected virtual void LinkControls()
        {
            m_kGrpBase = m_kForm.gbBase;
            m_kBtnAdd = m_kForm.btnAdd;
            m_kBtnCancel = m_kForm.btnCancel;

            m_kCmbType = m_kForm.cbEventType;
            m_kEbTextKey = m_kForm.txtTextKey;
            m_kEbTime = m_kForm.txtTime;
        }
        public virtual String GetTextKeyType()
        {
            return "Unknown";
        }
        public virtual void OnSelected()
        {
        }
        public virtual void OnDeselected()
        {
        }
        public virtual  bool SetInfo(MSequence.MTextKey kInfo)
        {
            m_kInfo = kInfo;

            if (m_kInfo == null)
            {
                m_kBtnAdd.Text = "Add";
                return false;
            }

            m_kBtnAdd.Text = "Edit";

            m_kCmbType.Enabled = false;
            m_kEbTextKey.Text = kInfo.Text;
            m_kEbTime.Text = kInfo.Time.ToString("f3");

            return true;
        }
        public MSequence.MTextKey GetInfo()
        {
            return m_kInfo;
        }

        public virtual bool AddToAnimation()
        {
            if (m_kEbTextKey.Text.Trim().Length == 0)
            {
                m_kEbTextKey.Focus();
                MessageBox.Show("TextKey Name을 입력해주세요");
                return false;
            }

            return true;
        }
        
        protected MSequence.MTextKey m_kInfo;
    }
}
