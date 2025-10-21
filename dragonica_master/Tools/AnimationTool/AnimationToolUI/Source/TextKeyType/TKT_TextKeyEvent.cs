using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using NiManagedToolInterface;
using System.Drawing;

namespace AnimationToolUI.Source.TextKeyType
{
    public class TKT_TextKeyEvent  : TKT_Base
    {
        public override String GetTextKeyType()
        {
            return "TextKey";
        }
        public override void OnSelected()
        {
            ArrangeLayoutForSelected();
        }
        public override void OnDeselected()
        {
        }
        private void ArrangeLayoutForSelected()
        {
            Size kFormSize = m_kForm.Size;

            m_kBtnAdd.Location = new Point(m_kBtnAdd.Location.X, m_kGrpBase.Location.Y + m_kGrpBase.Size.Height + 10);
            m_kBtnCancel.Location = new Point(m_kBtnCancel.Location.X, m_kGrpBase.Location.Y + m_kGrpBase.Size.Height + 10);

            m_kForm.Size = new Size(kFormSize.Width, m_kBtnAdd.Location.Y + m_kBtnAdd.Size.Height + 30);
        }

        public override bool AddToAnimation()
        {
            if (base.AddToAnimation() == false)
            {
                return false;
            }

            // 수정해야 한다면, 기존 Event를 지우고 추가한다.
            if (m_kInfo != null)
            {
                MFramework.Instance.Animation.RemoveEvent(m_kInfo.Text, m_kInfo.Time,
                    MAnimation.EventType.TextKeyEvent);
            }

            float fTime = 0;
            if (m_kEbTime.Text.Length > 0)
            {
                fTime = float.Parse(m_kEbTime.Text.Trim());
            }

            return MFramework.Instance.Animation.AddTextKeyEvent(m_kEbTextKey.Text.Trim(), fTime);
        }

        public override bool SetInfo(MSequence.MTextKey kInfo)
        {
            if ( false == base.SetInfo(kInfo))
            {
                return false;
            }

            ((ComboBox)m_kCmbType).SelectedIndex = 0;

            return true;
        }

    }
}
