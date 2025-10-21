using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Panels
{
    public partial class RoomPanel : Form
    {
        public RoomPanel()
        {
            InitializeComponent();
        }
        private void RefreshBtn_Click(object sender, EventArgs e)
        {

            //  Get list from
            MEntity[] amSceneEntities = MFramework.Instance.Scene
                            .GetEntities();

            RoomList.Items.Clear();
            foreach (MEntity pmEntity in amSceneEntities)
            {
                if (pmEntity.PGProperty == MEntity.ePGProperty.PS_Room)
                {
                    RoomList.Items.Add(pmEntity);
                }
            };

            ObjectList.Items.Clear();
            PortalList.Items.Clear();

            SelectedRoomName.Text = "";
            SelectedRoomName.Invalidate();
        }

        private void RefreshObjectList(MEntity pmEntity)
        {
            ObjectList.Items.Clear();

            MComponent[] amComponents = pmEntity.GetComponents();
            foreach (MComponent pmComp in amComponents)
            {
                if (pmComp.Name.Equals("PS_Room") == false)
                {
                    continue;
                }

                uint i = 0;
                while (true)
                {
                    MEntity kEntity = pmComp.GetPropertyData("Objects", i) as MEntity;
                    if (kEntity == null)
                    {
                        break;
                    }
                    ObjectList.Items.Add(kEntity);
                    ++i;
                }

                break;
            }
        }
        private void RefreshPortalList(MEntity pmEntity)
        {
            PortalList.Items.Clear();

            MComponent[] amComponents = pmEntity.GetComponents();
            foreach (MComponent pmComp in amComponents)
            {
                if (pmComp.Name.Equals("PS_Room") == false)
                {
                    continue;
                }

                uint i = 0;
                while (true)
                {
                    MEntity kEntity = pmComp.GetPropertyData("Portals", i) as MEntity;
                    if (kEntity == null)
                    {
                        break;
                    }
                    PortalList.Items.Add(kEntity);
                    ++i;
                }

                break;
            }
        }

        private void RoomList_OnSelectedIndexChanged(object sender, EventArgs e)
        {
            if (RoomList.SelectedItem == null)
            {
                return;
            }
            SelectedRoomName.Text = RoomList.SelectedItem.ToString();
            SelectedRoomName.Invalidate();

            RefreshObjectList(RoomList.SelectedItem as MEntity);
            RefreshPortalList(RoomList.SelectedItem as MEntity);

        }

        private void ObjectListAddBtn_OnClick(object sender, EventArgs e)
        {
            ISelectionService pmSelectionService =
                ServiceProvider.Instance.GetService(
                typeof(ISelectionService)) as ISelectionService;

            MEntity[] amSelectedEntities = pmSelectionService.GetSelectedEntities();
            foreach (MEntity pmEntity in amSelectedEntities)
            {
                AddObjectToObjectList(pmEntity);
            }

        }

        private void AddObjectToObjectList(MEntity pmEntity)
        {
            if (pmEntity.PGProperty == MEntity.ePGProperty.PS_Portal ||
                pmEntity.PGProperty == MEntity.ePGProperty.PS_Room
                )
            {
                return;
            }

            if (ObjectList.Items.Contains(pmEntity))
            {
                return;
            }

            ObjectList.Items.Add(pmEntity);


            MEntity pmSelectedRoom = RoomList.SelectedItem as MEntity;


            MComponent[] amComponents = pmSelectedRoom.GetComponents();
            foreach (MComponent pmComp in amComponents)
            {
                if (pmComp.Name.Equals("PS_Room") == false)
                {
                    continue;
                }

                uint i = 0;
                while (true)
                {
                    MEntity kEntity = pmComp.GetPropertyData("Objects", i) as MEntity;
                    if (kEntity == null)
                    {
                        pmComp.SetPropertyData("Objects", pmEntity,i,false);
                        break;
                    }
                    ++i;
                }

                break;
            }

        }

        private void ObjectListBtn_RemoveBtn_OnClick(object sender, EventArgs e)
        {
            

            MEntity pmSelectedRoom = RoomList.SelectedItem as MEntity;

            int i = 0;
            while(true)
            {
                int iTotalItem = ObjectList.Items.Count;
                if (i >= iTotalItem)
                {
                    break;
                }

                if (ObjectList.GetSelected(i))
                {
                    MEntity pmSelectedEntity = ObjectList.Items[i] as MEntity;
                    if (pmSelectedEntity != null)
                    {
                        MComponent[] amComponents = pmSelectedRoom.GetComponents();
                        foreach (MComponent pmComp in amComponents)
                        {
                            if (pmComp.Name.Equals("PS_Room") == false)
                            {
                                continue;
                            }

                            uint j = 0;
                            while (true)
                            {
                                MEntity kEntity = pmComp.GetPropertyData("Objects", j) as MEntity;
                                if (kEntity == null)
                                {
                                    break;
                                }
                                if (kEntity == pmSelectedEntity)
                                {
                                    uint k = j;
                                    while (true)
                                    {
                                        MEntity kNextEntity = pmComp.GetPropertyData("Objects", k+1) as MEntity;
                                        pmComp.SetPropertyData("Objects", kNextEntity, k, false);
                                        if (kNextEntity == null)
                                        {
                                            break;
                                        }

                                        ++k;
                                    }
                                    
                                    break;
                                }

                                ++j;
                            }

                            break;
                        }

                        ObjectList.Items.Remove(pmSelectedEntity);
                        continue;
                    }
                }


                ++i;

            }

        }

        private void PortalAddBtn_OnClick(object sender, EventArgs e)
        {
            ISelectionService pmSelectionService =
                ServiceProvider.Instance.GetService(
                typeof(ISelectionService)) as ISelectionService;

            MEntity[] amSelectedEntities = pmSelectionService.GetSelectedEntities();
            foreach (MEntity pmEntity in amSelectedEntities)
            {
                AddObjectToPortalList(pmEntity);
            }

        }
        private void AddObjectToPortalList(MEntity pmEntity)
        {
            if (pmEntity == null)
            {
                return;
            }

            if (pmEntity.PGProperty != MEntity.ePGProperty.PS_Portal)
            {
                return;
            }

            if (PortalList.Items.Contains(pmEntity))
            {
                return;
            }

            PortalList.Items.Add(pmEntity);

            MEntity pmSelectedRoom = RoomList.SelectedItem as MEntity;
            if (pmSelectedRoom == null)
            {
                return;
            }

            MComponent[] amComponents = pmSelectedRoom.GetComponents();
            foreach (MComponent pmComp in amComponents)
            {
                if (pmComp == null || pmComp.Name.Equals("PS_Room") == false)
                {
                    continue;
                }

                uint i = 0;
                while (true)
                {
                    MEntity kEntity = pmComp.GetPropertyData("Portals", i) as MEntity;
                    if (kEntity == null)
                    {
                        pmComp.SetPropertyData("Portals", pmEntity, i, false);
                        break;
                    }
                    ++i;
                }

                break;
            }

        }

        private void PortalRemoveBtn_OnClick(object sender, EventArgs e)
        {


            MEntity pmSelectedRoom = RoomList.SelectedItem as MEntity;

            int i = 0;
            while (true)
            {
                int iTotalItem = PortalList.Items.Count;
                if (i >= iTotalItem)
                {
                    break;
                }

                if (PortalList.GetSelected(i))
                {
                    MEntity pmSelectedEntity = PortalList.Items[i] as MEntity;
                    if (pmSelectedEntity != null)
                    {
                        MComponent[] amComponents = pmSelectedRoom.GetComponents();
                        foreach (MComponent pmComp in amComponents)
                        {
                            if (pmComp.Name.Equals("PS_Room") == false)
                            {
                                continue;
                            }

                            uint j = 0;
                            while (true)
                            {
                                MEntity kEntity = pmComp.GetPropertyData("Portals", j) as MEntity;
                                if (kEntity == null)
                                {
                                    break;
                                }
                                if (kEntity == pmSelectedEntity)
                                {
                                    uint k = j;
                                    while (true)
                                    {
                                        MEntity kNextEntity = pmComp.GetPropertyData("Portals", k + 1) as MEntity;
                                        pmComp.SetPropertyData("Portals", kNextEntity, k, false);
                                        if (kNextEntity == null)
                                        {
                                            break;
                                        }

                                        ++k;
                                    }

                                    break;
                                }

                                ++j;
                            }

                            break;
                        }

                        PortalList.Items.Remove(pmSelectedEntity);
                        continue;
                    }
                }


                ++i;

            }
        }
    }
}