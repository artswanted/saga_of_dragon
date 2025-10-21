using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using NiManagedToolInterface;
using System.Drawing;

namespace AnimationToolUI.Source.TextKeyType
{
    class TKT_FadeOutEffectEvent : TKT_Base
    {
        enum CONTROL_ID
        {
            CID_GRP_FADEOUTEFFECT = 0,
            CID_LB_FADEOUTEFFECT_NAME,
            CID_CB_FADEOUTEFFECT_NAME,
            CID_LB_FADEOUTTIME,
            CID_EB_FADEOUTTIME,
            CID_MAX
        };

        private Control[] m_kControls = new Control[(int)(CONTROL_ID.CID_MAX)];

        protected override void LinkControls()
        {
            base.LinkControls();

            m_kControls[(int)CONTROL_ID.CID_GRP_FADEOUTEFFECT] = m_kForm.gbFadeOutEffect;
            m_kControls[(int)CONTROL_ID.CID_LB_FADEOUTEFFECT_NAME] = m_kForm.lbFadeOutEffectName;
            m_kControls[(int)CONTROL_ID.CID_CB_FADEOUTEFFECT_NAME] = m_kForm.cbEffectName;
            m_kControls[(int)CONTROL_ID.CID_LB_FADEOUTTIME] = m_kForm.lbFadeOutEffectTime;
            m_kControls[(int)CONTROL_ID.CID_EB_FADEOUTTIME] = m_kForm.tbFadeOutTime;

        }
        public override String GetTextKeyType()
        {
            return "FadeOutEffectKey";
        }
        public override void OnSelected()
        {
            ArrangeLayoutForSelected();
            ShowControls();
            InitEffectCombo();
        }
        public override void OnDeselected()
        {
            HideControls();
        }

        private void ShowControls()
        {
            for (uint ui = 0; ui < (int)CONTROL_ID.CID_MAX; ++ui)
            {
                m_kControls[ui].Show();
            }
        }
        private void HideControls()
        {
            for (uint ui = 0; ui < (int)CONTROL_ID.CID_MAX; ++ui)
            {
                m_kControls[ui].Hide();
            }
        }
        private void ArrangeLayoutForSelected()
        {
            Size kFormSize = m_kForm.Size;

            Point kLeftTopCorner = new Point(m_kGrpBase.Location.X, m_kGrpBase.Location.Y + m_kGrpBase.Size.Height);
            m_kControls[(int)CONTROL_ID.CID_GRP_FADEOUTEFFECT].Location = new Point(m_kControls[(int)CONTROL_ID.CID_GRP_FADEOUTEFFECT].Location.X, kLeftTopCorner.Y + 10);

            m_kBtnAdd.Location = new Point(m_kBtnAdd.Location.X, m_kControls[(int)CONTROL_ID.CID_GRP_FADEOUTEFFECT].Location.Y + m_kControls[(int)CONTROL_ID.CID_GRP_FADEOUTEFFECT].Size.Height + 10);
            m_kBtnCancel.Location = new Point(m_kBtnCancel.Location.X, m_kControls[(int)CONTROL_ID.CID_GRP_FADEOUTEFFECT].Location.Y + m_kControls[(int)CONTROL_ID.CID_GRP_FADEOUTEFFECT].Size.Height + 10);

            m_kForm.Size = new Size(kFormSize.Width, m_kBtnAdd.Location.Y + m_kBtnAdd.Size.Height + 30);
        }
        public override bool SetInfo(MSequence.MTextKey kInfo)
        {
            if (base.SetInfo(kInfo) == false)
            {
                return false;
            }

            ((ComboBox)m_kCmbType).SelectedIndex = 3;

            MSequence.MTextKey.MFadeOutEffectData kFadeOutEffectData = (MSequence.MTextKey.MFadeOutEffectData)kInfo.ExtraData;
            m_kControls[(int)CONTROL_ID.CID_EB_FADEOUTTIME].Text = kFadeOutEffectData.FadeOutTime.ToString("f3");

            SelectEffect(kFadeOutEffectData.EffectName);

            return true;
        }
        private void SelectEffect(String kEffectName)
        {
            int iIndex = ((ComboBox)m_kControls[(int)CONTROL_ID.CID_CB_FADEOUTEFFECT_NAME]).FindString(kEffectName, -1);
            if (iIndex >= 0)
            {
                ((ComboBox)m_kControls[(int)CONTROL_ID.CID_CB_FADEOUTEFFECT_NAME]).SelectedIndex = iIndex;
            }
        }
        private bool InitEffectCombo()
        {
            ((ComboBox)m_kControls[(int)CONTROL_ID.CID_CB_FADEOUTEFFECT_NAME]).Items.Clear();

            MSequence kSequence = MFramework.Instance.Animation.ActiveSequence;

            MSequence.MTextKey[] kTextKeys = kSequence.TextKeys;

            foreach (MSequence.MTextKey kTextKey in kTextKeys)
            {
                if (kTextKey.ExtraData is MSequence.MTextKey.MEffectData)
                {
                    ((ComboBox)m_kControls[(int)CONTROL_ID.CID_CB_FADEOUTEFFECT_NAME]).Items.Add(kTextKey.Text);
                }
            }

            if (((ComboBox)m_kControls[(int)CONTROL_ID.CID_CB_FADEOUTEFFECT_NAME]).Items.Count == 0)
            {
                m_kControls[(int)CONTROL_ID.CID_CB_FADEOUTEFFECT_NAME].Enabled = false;
                m_kControls[(int)CONTROL_ID.CID_EB_FADEOUTTIME].Enabled = false;
                MessageBox.Show("현재 시퀀스에 Effect 가 존재하지 않습니다.");
                return false;
            }

            return true;
        }
        public override bool AddToAnimation()
        {
            if (base.AddToAnimation() == false)
            {
                return false;
            }

            if (m_kControls[(int)CONTROL_ID.CID_CB_FADEOUTEFFECT_NAME].Text.Trim().Length == 0)
            {
                m_kControls[(int)CONTROL_ID.CID_CB_FADEOUTEFFECT_NAME].Focus();
                MessageBox.Show("Effect Symbol을 입력해주세요.");
                return false;
            }

            // 수정해야 한다면, 기존 Event를 지우고 추가한다.
            if (m_kInfo != null)
            {
                MFramework.Instance.Animation.RemoveEvent(m_kInfo.Text, m_kInfo.Time,
                    MAnimation.EventType.FadeOutEffectEvent);
                MFramework.Instance.Animation.ResetAnimations();
            }

            float fFadeOutTime = 0;
            if (m_kControls[(int)CONTROL_ID.CID_EB_FADEOUTTIME].Text.Length > 0)
            {
                fFadeOutTime = float.Parse(m_kControls[(int)CONTROL_ID.CID_EB_FADEOUTTIME].Text.Trim());
            }

            return MFramework.Instance.Animation.AddFadeOutEffectEvent(
                m_kEbTextKey.Text.Trim(),
                float.Parse(m_kEbTime.Text.Trim()),
                m_kControls[(int)CONTROL_ID.CID_CB_FADEOUTEFFECT_NAME].Text.Trim(),
                fFadeOutTime
                );
        }
    }
}
