using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using NiManagedToolInterface;
using System.Drawing;

namespace AnimationToolUI.Source.TextKeyType
{
    class TKT_SoundEvent : TKT_Base
    {
        enum CONTROL_ID
        {
            CID_GRP_SOUND = 0,
            CID_LB_SOUND_NAME,
            CID_EB_SOUND_NAME,
            CID_BTN_SOUND_BROWSE,
            CID_LB_VOLUME,
            CID_EB_VOLUME,
            CID_LB_MIN_DISTANCE,
            CID_EB_MIN_DISTANCE,
            CID_LB_MAX_DISTANCE,
            CID_EB_MAX_DISTANCE,
            CID_MAX
        };

        private Control[] m_kControls = new Control[(int)(CONTROL_ID.CID_MAX)];

        protected override void LinkControls()
        {
            base.LinkControls();

            m_kControls[(int)CONTROL_ID.CID_GRP_SOUND] = m_kForm.gbSound;
            m_kControls[(int)CONTROL_ID.CID_LB_SOUND_NAME] = m_kForm.lbSoundName;
            m_kControls[(int)CONTROL_ID.CID_EB_SOUND_NAME] = m_kForm.txtSoundName;
            m_kControls[(int)CONTROL_ID.CID_BTN_SOUND_BROWSE] = m_kForm.btnBrowseSound;
            m_kControls[(int)CONTROL_ID.CID_LB_VOLUME] = m_kForm.lbVolume;
            m_kControls[(int)CONTROL_ID.CID_EB_VOLUME] = m_kForm.nsVolume;
            m_kControls[(int)CONTROL_ID.CID_LB_MIN_DISTANCE] = m_kForm.lbMinDistance;
            m_kControls[(int)CONTROL_ID.CID_EB_MIN_DISTANCE] = m_kForm.nsMinDistance;
            m_kControls[(int)CONTROL_ID.CID_LB_MAX_DISTANCE] = m_kForm.lbMaxDistance;
            m_kControls[(int)CONTROL_ID.CID_EB_MAX_DISTANCE] = m_kForm.nsMaxDistance;

        }
        public override String GetTextKeyType()
        {
            return "SoundKey";
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

            m_kControls[(int)CONTROL_ID.CID_GRP_SOUND].Location = new Point(m_kControls[(int)CONTROL_ID.CID_GRP_SOUND].Location.X,kLeftTopCorner.Y + 10);

            m_kBtnAdd.Location = new Point(m_kBtnAdd.Location.X, m_kControls[(int)CONTROL_ID.CID_GRP_SOUND].Location.Y + m_kControls[(int)CONTROL_ID.CID_GRP_SOUND].Size.Height + 10);
            m_kBtnCancel.Location = new Point(m_kBtnCancel.Location.X, m_kControls[(int)CONTROL_ID.CID_GRP_SOUND].Location.Y + m_kControls[(int)CONTROL_ID.CID_GRP_SOUND].Size.Height + 10);

            m_kForm.Size = new Size(kFormSize.Width, m_kBtnAdd.Location.Y + m_kBtnAdd.Size.Height + 30);
        }
        public override bool SetInfo(MSequence.MTextKey kInfo)
        {
            if (base.SetInfo(kInfo) == false)
            {
                return false;
            }

            ((ComboBox)m_kCmbType).SelectedIndex = 2;

            MSequence.MTextKey.MAudioData kAudioData = (MSequence.MTextKey.MAudioData)kInfo.ExtraData;
            m_kControls[(int)CONTROL_ID.CID_EB_SOUND_NAME].Text = kAudioData.SoundName;
            ((NumericSlider)m_kControls[(int)CONTROL_ID.CID_EB_VOLUME]).Value = new decimal(kAudioData.Volume);
            ((NumericSlider)m_kControls[(int)CONTROL_ID.CID_EB_MIN_DISTANCE]).Value = new decimal(kAudioData.MinDistance);
            ((NumericSlider)m_kControls[(int)CONTROL_ID.CID_EB_MAX_DISTANCE]).Value = new decimal(kAudioData.MaxDistance);

            return true;
        }
        public override bool AddToAnimation()
        {
            if (base.AddToAnimation() == false)
            {
                return false;
            }

            if (m_kControls[(int)CONTROL_ID.CID_EB_SOUND_NAME].Text.Trim().Length == 0)
            {
                m_kControls[(int)CONTROL_ID.CID_EB_SOUND_NAME].Focus();
                MessageBox.Show("Sound Symbol을 입력해주세요.");
                return false;
            }

            // 수정해야 한다면, 기존 Event를 지우고 추가한다.
            if (m_kInfo != null)
            {
                MFramework.Instance.Animation.RemoveEvent(m_kInfo.Text, m_kInfo.Time,
                    MAnimation.EventType.SoundEvent);
                MFramework.Instance.Animation.ResetAnimations();
            }

            return MFramework.Instance.Animation.AddSoundEvent(
                m_kEbTextKey.Text.Trim(),
                float.Parse(m_kEbTime.Text.Trim()),
                m_kControls[(int)CONTROL_ID.CID_EB_SOUND_NAME].Text,
                Convert.ToSingle(((NumericSlider)m_kControls[(int)CONTROL_ID.CID_EB_VOLUME]).Value),
                Convert.ToSingle(((NumericSlider)m_kControls[(int)CONTROL_ID.CID_EB_MIN_DISTANCE]).Value),
                Convert.ToSingle(((NumericSlider)m_kControls[(int)CONTROL_ID.CID_EB_MAX_DISTANCE]).Value));
        }
    }
}
