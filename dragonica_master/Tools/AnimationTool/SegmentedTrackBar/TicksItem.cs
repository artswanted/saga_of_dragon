using System;
using System.Drawing;

namespace SegmentedTrackBarDLL
{
	/// <summary>
	/// Summary description for Ticks.
	/// </summary>
    public sealed class TicksItem: SegmentedTrackBarDLL.CollectionItem
	{
        private int m_iStartPos;
        private int m_iWidth;
        private int m_iLevel;
        private SegmentedTrackBar m_TrackBar = null;

		public TicksItem()
		{
            m_iStartPos = 0;
            m_iWidth = 0;
            m_iLevel = 0;
		}

        public int StartPos
        {
            get { return m_iStartPos; }
            set { m_iStartPos = value; }
        }

        public int Width
        {
            get { return m_iWidth; }
            set { m_iWidth = value; }
        }

        public int Level
        {
            get { return m_iLevel; }
            set { m_iLevel = value; }
        }

        public void SetTrackBar(SegmentedTrackBar trackBar)
        {
            m_TrackBar = trackBar;
        }

        public void Draw()
        {
            if (m_TrackBar == null)
                return;

            if (Width <= 0)
                return;

            //if (m_bVisible == false)
            //    return;

            Pen drawingPen = new Pen(Color.Black, 1);

            int iPos = m_iStartPos;
            Point pt1, pt2;
            while (iPos < m_TrackBar.MaxRange)
            {
                int iPixel = m_TrackBar.ConvertPosToPixel(iPos);
                pt1 = new Point(iPixel, m_TrackBar.VertPadding + 
                    m_TrackBar.LevelHeight * this.m_iLevel);
                pt2 = new Point(iPixel, pt1.Y + m_TrackBar.LevelHeight);
                m_TrackBar.m_DBGraphics.g.DrawLine(drawingPen,pt1,pt2);
                iPos += Width;
            }
        }
	}

    public class TicksCollection: SegmentedTrackBarDLL.Collection
    {
        public Int32 Add(TicksItem item)
        {
            return base.Add(item);
        }

        public void Insert(Int32 index, TicksItem item)
        {
            base.Insert(index, item);
        }

        public void Remove(TicksItem item)
        {
            base.Remove(item);
            ((IDisposable)item).Dispose();
        }

        public bool Contains(TicksItem item)
        {
            return base.Contains(item);
        }

        public Int32 IndexOf(TicksItem item)
        {
            return base.IndexOf(item);
        }

        public void CopyTo(TicksItem[] array, Int32 index)
        {
            base.CopyTo(array, index);
        }

        public new TicksItem this[Int32 index]
        {
            get
            {
                return (TicksItem)List[index];
            }
            set
            {
                List[index] = value;
            }
        }
    }
}
