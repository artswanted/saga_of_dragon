// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2006 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

using System;
using System.IO;
using System.Xml;
using System.Xml.Schema;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.UIBinding
{
	/// <summary>
	/// Summary description for IBuilder.
	/// </summary>
	internal abstract class AbstractBuilder
	{
        #region PrivateData
        protected XmlSchema m_schema;
        #endregion

        public XmlSchema Schema
        {
            get
            { return m_schema; }
            set
            { m_schema = value; }
        }
        public abstract void Build(Stream xmlStream);

        protected XmlDocument ReadXML(Stream stream)
        {
            XmlTextReader reader = new XmlTextReader(stream);
            XmlValidatingReader validatingReader = new XmlValidatingReader(reader);

            if (m_schema != null)
            {
                validatingReader.Schemas.Add(m_schema);
            }
            
            XmlDocument doc = new XmlDocument();
            doc.Load(validatingReader);
            return doc;
        }
	}


}
