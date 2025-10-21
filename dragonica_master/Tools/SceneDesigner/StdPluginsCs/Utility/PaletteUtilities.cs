using System;
using System.Collections;
using System.IO;
using System.Text;
using System.Text.RegularExpressions;
using System.Windows.Forms;
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;
using Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Panels;
using Message = Emergent.Gamebryo.SceneDesigner.PluginAPI.Message;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Utility
{
	/// <summary>
	/// Summary description for PaletteUtilities.
	/// </summary>
	public class PaletteUtilities
	{
		private PaletteUtilities()//utlity/static class no constructor
		{
		}

        /// <summary>
        /// 
        /// </summary>
        /// <param name="palette"></param>
        /// <param name="category"></param>
        /// <returns>An array of the full paths of all immediate decendents
        /// of the passed in path</returns>
        internal static string[] BuildSubcategories(MPalette palette, string category)
        {
            //contains the category entries for all entities in palette
            ArrayList masterCategoryList = new ArrayList();
            MEntity[] entities = palette.GetEntities();
            foreach (MEntity entity in entities)
            {
                string entityCategory = palette.GetEntityCategory(entity);
                if (//entityCategory != null && 
                    !entityCategory.Equals(string.Empty))
                {
                    masterCategoryList.Add(entityCategory);
                }
            }

            ArrayList categoryList = new ArrayList();

            foreach (string entityCategory in masterCategoryList)
            {
                string[] simpleCategories = entityCategory.Split('.');
                StringBuilder builder = new StringBuilder();
                for (int i = 0; i < simpleCategories.Length; i++)
                {
                    builder.Append(simpleCategories[i]);
                    string localCategory = builder.ToString();
                    string parentCategory = ParentCategory(localCategory);
                    if (parentCategory.Equals(category))
                    {
                        if (!categoryList.Contains(localCategory))
                        {
                            categoryList.Add(localCategory);
                        }
                        break; //no need to continue searching
                    }
                    builder.Append(".");
                }
            }
            return categoryList.ToArray(typeof(string)) as string[];
        }

        internal static string ParentCategory(string fullName)
        {
            string fullNameWithoutPalette = MPalette.StripPaletteName(fullName);
            string simpleCategory = SimpleName(fullName);
            if (fullNameWithoutPalette.Equals(simpleCategory))
            {
                return string.Empty;
            }
            return fullNameWithoutPalette.Substring(0, 
                fullNameWithoutPalette.LastIndexOf(simpleCategory)-1);
        }

        internal static string SimpleName(string fullName)
        {
            string namewithoutPalette = MPalette.StripPaletteName(fullName);
            Regex regex = new Regex(@"[.]");
            string[] categories = regex.Split(namewithoutPalette);
            return categories[categories.Length - 1];
        }


	    internal static string FullCategoryName(string[] categories)
        {
            if (categories.Length > 0)
            {
                StringBuilder sb = new StringBuilder();
                sb.Append(categories[0]);
                for(int i = 1; i < categories.Length; i++)
                {
                    sb.AppendFormat(".{0}", categories[i]);
                }
                return sb.ToString();
            }
            return string.Empty;                     
        }

        internal static void ImportPalettes(string[] filenames)
        {

            MFramework fw = MFramework.Instance;
            //string destinationFolderPath = fw.PaletteManager.PaletteFolder;
            foreach(string filename in filenames)
            {
                FileInfo fileInfo = new FileInfo(filename);
                string paletteName = //GetUniqueName(
                    fileInfo.Name.Replace(".pal", "");
                string filepath = filename.Replace(fileInfo.Name ,"");

                //string newPath = destinationFolderPath + paletteName + ".pal";
                //fileInfo.CopyTo(newPath);
                MPalette destinationPalette =
                    fw.PaletteManager.GetPaletteByName(paletteName);
                if (destinationPalette == null)
                {
                    MPalette palette =
                        fw.PaletteManager.LoadPalette(filename);
                    //destinationPalette = new MPalette(paletteName, 0);                    

                    if (palette != null)
                    {
                        palette.ResetEntityNames();
                        //DirtyBitUtilities.MakeSceneClean(palette.Scene);

                        MEntity[] newTemplates = palette.GetEntities();

                        foreach (MEntity template in newTemplates)
                        {
                            //TODO make non-undoable
                            palette.AddEntity(template, string.Empty,
                                true);
                        } 
                        
                        fw.PaletteManager.AddPalette(palette);
                    }
                }
                else
                {
                    MessageBox.Show("팔레트 리스트 내에 이미 해당 팔레트가 있습니다.");
                }

                return;
            }
        }


	    private static string GetUniqueName(string paletteName)
	    {
            MFramework fw = MFramework.Instance;
            if (fw.PaletteManager.GetPaletteByName(paletteName) == null)
            {
                return paletteName;
            }
	        MPalette[] existingPalettes = fw.PaletteManager.GetPalettes();
            ArrayList existingPaletteNames = new ArrayList();
            foreach (MPalette palette in existingPalettes)
            {
                existingPaletteNames.Add(palette.Name.ToUpper());
            }
            int index = 1;
            while (true)
            {
                string possibleName = string.Format("{0}_{1:00#}",
                    paletteName, index);
                if (!existingPaletteNames.Contains(possibleName.ToUpper()))
                {
                    return possibleName;
                }
                index++;
            }
	    }

        private class RemovalRecord
        {
            public MPalette palette;
            public MEntity entity;
        }

        internal static bool CheckForDuplicateTemplates(MPalette palette, 
            MPalette[] existingPalettes)
        {
            TemplateConflictDlg dlg = new TemplateConflictDlg();

            ServiceProvider sp = ServiceProvider.Instance;
            IEntityPathService pathService = sp.GetService(
                typeof(IEntityPathService)) as IEntityPathService;
            ArrayList removalList = new ArrayList();
            ArrayList additionList = new ArrayList();
            MEntity[] paletteEntities = palette.Scene.GetEntities();
            foreach (MEntity paletteEntity in paletteEntities)
            {
                foreach (MPalette existingPalette in existingPalettes)
                {
                    MEntity[] existingEntities = 
                        existingPalette.Scene.GetEntities();
                    foreach(MEntity existingEntity in existingEntities)
                    {
                        //Then a conflict was found
                        if (paletteEntity.TemplateID == 
                            existingEntity.TemplateID)
                        {
                            if (existingPalette.DontSave)
                            {//Then the conflict is with a temporary palette
                                //pulll the existing entity from the 
                                //temporary palette and place it in the 
                                //imported one
                                RemovalRecord existingrecord = 
                                    new RemovalRecord();
                                existingrecord.palette = existingPalette;
                                existingrecord.entity = existingEntity;
                                removalList.Add(existingrecord);
                                RemovalRecord record = new RemovalRecord();
                                record.palette = palette;
                                record.entity = paletteEntity;
                                removalList.Add(record);
                                additionList.Add(existingEntity);
                                continue;
                            }
                            string msg = string.Format("Template '{0}'" +
                                " conflicts with existing template '{1}'",
                                pathService.GetFullPath(
                                palette.Scene, paletteEntity),
                                pathService.GetFullPath(
                                existingPalette.Scene, existingEntity));

                            dlg.Message = msg;
                            if (!dlg.ApplyToAll)
                            {
                                dlg.ShowDialog();
                            }
                            switch (dlg.Action)
                            {
                                case TemplateConflictDlg.ConflictAction.Skip: 
                                {
                                    RemovalRecord record = new RemovalRecord();
                                    record.palette = palette;
                                    record.entity = paletteEntity;
                                    removalList.Add(record);
                                    break;
                                }
                                case TemplateConflictDlg.ConflictAction.Clone: 
                                {
                                    RemovalRecord record = new RemovalRecord();
                                    record.palette = palette;
                                    record.entity = paletteEntity;
                                    removalList.Add(record);
                                    //removalList.Add(paletteEntity);
                                    MEntity clone = paletteEntity.Clone(
                                        paletteEntity.Name, false);
                                    clone.TemplateID = Guid.NewGuid();
                                    additionList.Add(clone);
                                    break;
                                }
                                case TemplateConflictDlg.ConflictAction.Replace:
                                {
                                    RemovalRecord record = new RemovalRecord();
                                    record.palette = existingPalette;
                                    record.entity = existingEntity;
                                    removalList.Add(record);
                                    additionList.Add(paletteEntity);
                                    break;
                                }
                                case TemplateConflictDlg.ConflictAction.Cancel: 
                                {
                                    return false;
                                }
                            }
                        }
                    }
                }
            }
            
            foreach(RemovalRecord record in removalList)
            {
                record.palette.Scene.RemoveEntity(record.entity, false);
            }
            foreach(MEntity entityToAdd in additionList)
            {
                palette.Scene.AddEntity(entityToAdd, false);
            }
            return true;
        }


	}
}
