using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using NiManagedToolInterface;
using System.Drawing;

namespace AnimationToolUI.Source.TextKeyType
{
    class TKT_EffectEvent : TKT_Base
    {
        enum CONTROL_ID
        {
            CID_GRP_EFFECT=0,
            CID_LB_EFFECT_NAME,
            CID_EB_EFFECT_NAME,
            CID_BTN_EFFECT_BROWSE,
            CID_LB_ATTACHPOINT,
            CID_CMB_ATTACHPOINT,
            CID_BTN_ADD_ATTACHPOINT,
            CID_BTN_REMOVE_ATTACHPOINT,
            CID_LB_SACLE,
            CID_EB_SCALE,
            CID_LB_FADEIN,
            CID_TB_FADEINTIME,
            CID_CHK_USEFADEIN,
            CID_LB_FADEOUTWHENSEQUENCECHANGED,
            CID_TB_FADEOUTTIMEWHENSEQUENCECHANGED,
            CID_CHK_USEFADEOUTWHENSEQUENCECHANGED,
            CID_MAX
        };

        private Control[] m_kControls = new Control[(int)(CONTROL_ID.CID_MAX)];

        protected override void LinkControls()
        {
            base.LinkControls();

            m_kControls[(int)CONTROL_ID.CID_GRP_EFFECT] = m_kForm.gbEffect;
            m_kControls[(int)CONTROL_ID.CID_LB_EFFECT_NAME] = m_kForm.lbEffectName;
            m_kControls[(int)CONTROL_ID.CID_EB_EFFECT_NAME] = m_kForm.txtEffectName;
            m_kControls[(int)CONTROL_ID.CID_BTN_EFFECT_BROWSE] = m_kForm.btnBrowseEffect;
            m_kControls[(int)CONTROL_ID.CID_LB_ATTACHPOINT] = m_kForm.lbAttachPoint;
            m_kControls[(int)CONTROL_ID.CID_CMB_ATTACHPOINT] = m_kForm.cbAttachPoint;
            m_kControls[(int)CONTROL_ID.CID_BTN_ADD_ATTACHPOINT] = m_kForm.Btn_AddAttachPoint;
            m_kControls[(int)CONTROL_ID.CID_BTN_REMOVE_ATTACHPOINT] = m_kForm.Btn_RemoveAttachPoint;
            m_kControls[(int)CONTROL_ID.CID_LB_SACLE] = m_kForm.lbScale;
            m_kControls[(int)CONTROL_ID.CID_EB_SCALE] = m_kForm.nsEffectScale;
            m_kControls[(int)CONTROL_ID.CID_LB_FADEIN] = m_kForm.lbFadeIn;
            m_kControls[(int)CONTROL_ID.CID_TB_FADEINTIME] = m_kForm.tbFadeInTime;
            m_kControls[(int)CONTROL_ID.CID_CHK_USEFADEIN] = m_kForm.cbUseFadeIn;
            m_kControls[(int)CONTROL_ID.CID_LB_FADEOUTWHENSEQUENCECHANGED] = m_kForm.lbFadeOutWhenSequenceChanged;
            m_kControls[(int)CONTROL_ID.CID_TB_FADEOUTTIMEWHENSEQUENCECHANGED] = m_kForm.tbFadeOutTimeWhenSequenceChanged;
            m_kControls[(int)CONTROL_ID.CID_CHK_USEFADEOUTWHENSEQUENCECHANGED] = m_kForm.cbFadeOutWhenSequenceChanged;

        }
        public override String GetTextKeyType()
        {
            return "EffectKey";
        }
        public override void OnSelected()
        {
            ArrangeLayoutForSelected();
            ShowControls();
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
            m_kControls[(int)CONTROL_ID.CID_GRP_EFFECT].Location = new Point(m_kControls[(int)CONTROL_ID.CID_GRP_EFFECT].Location.X, kLeftTopCorner.Y + 10);

            m_kBtnAdd.Location = new Point(m_kBtnAdd.Location.X, m_kControls[(int)CONTROL_ID.CID_GRP_EFFECT].Location.Y + m_kControls[(int)CONTROL_ID.CID_GRP_EFFECT].Size.Height + 10);
            m_kBtnCancel.Location = new Point(m_kBtnCancel.Location.X, m_kControls[(int)CONTROL_ID.CID_GRP_EFFECT].Location.Y + m_kControls[(int)CONTROL_ID.CID_GRP_EFFECT].Size.Height + 10);

            m_kForm.Size = new Size(kFormSize.Width, m_kBtnAdd.Location.Y + m_kBtnAdd.Size.Height + 30);
        }
        public override bool SetInfo(MSequence.MTextKey kInfo)
        {
            if (base.SetInfo(kInfo) == false)
            {
                return false;
            }

            ((ComboBox)m_kCmbType).SelectedIndex = 1;

            MSequence.MTextKey.MEffectData kEffectData = (MSequence.MTextKey.MEffectData)kInfo.ExtraData;
            m_kControls[(int)CONTROL_ID.CID_EB_EFFECT_NAME].Text = kEffectData.EffectName;
            AddAttachPoint(kEffectData.AttachPoint);
            SetAttachPointSelectedIndex(((ComboBox)m_kControls[(int)CONTROL_ID.CID_CMB_ATTACHPOINT]).FindString(kEffectData.AttachPoint, 0));
            m_kControls[(int)CONTROL_ID.CID_BTN_REMOVE_ATTACHPOINT].Show();
            ((NumericSlider)m_kControls[(int)CONTROL_ID.CID_EB_SCALE]).Value = new decimal(kEffectData.Scale);
            ((CheckBox)m_kControls[(int)CONTROL_ID.CID_CHK_USEFADEIN]).Checked = kEffectData.UseFadeIn;
            m_kControls[(int)CONTROL_ID.CID_TB_FADEINTIME].Text = kEffectData.FadeInTime.ToString("f3");

            ((CheckBox)m_kControls[(int)CONTROL_ID.CID_CHK_USEFADEOUTWHENSEQUENCECHANGED]).Checked = kEffectData.UseFadeOutWhenSequenceChanged;
            m_kControls[(int)CONTROL_ID.CID_TB_FADEOUTTIMEWHENSEQUENCECHANGED].Text = kEffectData.FadeOutTimeWhenSequenceChanged.ToString("f3");
            return true;
        }
        private bool AddAttachPoint(String Text)
        {
            MFramework.Instance.AttachPointList.AddAttachPoint(Text);
            if ( ((ComboBox)m_kControls[(int)CONTROL_ID.CID_CMB_ATTACHPOINT]).FindString(Text, 0) == -1)
            {
                ((ComboBox)m_kControls[(int)CONTROL_ID.CID_CMB_ATTACHPOINT]).Items.Add(Text);
            }
            return true;
        }
        private void SetAttachPointSelectedIndex(int uiIndex)
        {
            if (uiIndex >= ((ComboBox)m_kControls[(int)CONTROL_ID.CID_CMB_ATTACHPOINT]).Items.Count)
            {
                return;
            }

            ((ComboBox)m_kControls[(int)CONTROL_ID.CID_CMB_ATTACHPOINT]).SelectedIndex = uiIndex;
        }
        public override bool AddToAnimation()
        {
            if (base.AddToAnimation() == false)
            {
                return false;
            }

            if (m_kControls[(int)CONTROL_ID.CID_EB_EFFECT_NAME].Text.Trim().Length == 0)
            {
                m_kControls[(int)CONTROL_ID.CID_EB_EFFECT_NAME].Focus();
                MessageBox.Show("Effect Symbol을 입력해주세요.");
                return false;
            } 

            // 수정해야 한다면, 기존 Event를 지우고 추가한다.
            if (m_kInfo != null)
            {
                MFramework.Instance.Animation.RemoveEvent(m_kInfo.Text, m_kInfo.Time,
                    MAnimation.EventType.EffectEvent);
                MFramework.Instance.Animation.ResetAnimations();
            }

            float fFadeInTime = 0;
            if (m_kControls[(int)CONTROL_ID.CID_TB_FADEINTIME].Text.Length > 0)
            {
                fFadeInTime = float.Parse(m_kControls[(int)CONTROL_ID.CID_TB_FADEINTIME].Text.Trim());
            }
            float fFadeOutTime = 0;
            if (m_kControls[(int)CONTROL_ID.CID_TB_FADEOUTTIMEWHENSEQUENCECHANGED].Text.Length > 0)
            {
                fFadeOutTime = float.Parse(m_kControls[(int)CONTROL_ID.CID_TB_FADEOUTTIMEWHENSEQUENCECHANGED].Text.Trim());
            }

            return MFramework.Instance.Animation.AddEffectEvent(
                m_kEbTextKey.Text.Trim(),
                float.Parse(m_kEbTime.Text.Trim()), 
                m_kControls[(int)CONTROL_ID.CID_EB_EFFECT_NAME].Text.Trim(), 
                m_kControls[(int)CONTROL_ID.CID_CMB_ATTACHPOINT].Text,
                Convert.ToSingle(((NumericSlider)m_kControls[(int)CONTROL_ID.CID_EB_SCALE]).Value), 
                fFadeInTime,
                ((CheckBox)m_kControls[(int)CONTROL_ID.CID_CHK_USEFADEIN]).Checked,
                fFadeOutTime,
                 ((CheckBox)m_kControls[(int)CONTROL_ID.CID_CHK_USEFADEOUTWHENSEQUENCECHANGED]).Checked
                );
        }
    }
}
