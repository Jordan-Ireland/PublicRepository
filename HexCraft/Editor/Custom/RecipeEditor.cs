using System.Collections;
using System.Collections.Generic;
using UnityEditor;
using UnityEngine;

namespace AFG.Editors
{
    public class RecipeEditor : BaseEditor
    {
        public ItemDatabase itemDatabase;
        RecipeDatabase database;
        RecipeBook tempRecipeBook;

        List<string> itemsNames = new List<string>();

        [MenuItem("Custom/Recipe Editor")]
        public static void Init()
        {
            RecipeEditor window = GetWindow<RecipeEditor>();
            window.minSize = new Vector2(800, 600);
            window.titleContent.text = "Recipe Editor";
            window.Show();
        }

        protected override void OnEnable()
        {
            base.OnEnable();
            UpdateList();
        }

        protected override void UpdateList()
        {
            base.UpdateList();
            itemsNames = new List<string>();
            itemDatabase = Resources.Load("Databases/ItemDatabase") as ItemDatabase;
            database = Resources.Load("Databases/RecipeDatabase") as RecipeDatabase;
            for (int i = 0; i < itemDatabase.Count; i++)
            {
                itemsNames.Add(itemDatabase.database[i].Name);
            }
        }

        protected override void OnFocus()
        {
            base.OnFocus();
        }

        protected override void OnGUI()
        {
            base.OnGUI();
        }

        protected override void ListView()
        {
            base.ListView();
        }

        protected override void ItemDetails()
        {
            base.ItemDetails();
            GUILayout.BeginVertical("Box", GUILayout.ExpandWidth(true), GUILayout.ExpandHeight(true));
            GUILayout.BeginVertical(GUILayout.ExpandWidth(true), GUILayout.ExpandHeight(true));

            if (showDetails)
            {
                GUILayout.BeginVertical();
                
                GUILayout.BeginHorizontal();
                GUILayout.Label("Name: ");
                GUILayout.Label("ID: " + tempRecipeBook.id);
                GUILayout.EndHorizontal();

                GUILayout.BeginHorizontal();
                GUILayout.Label("Name: ");
                tempRecipeBook.name = GUILayout.TextField(tempRecipeBook.name);
                GUILayout.EndHorizontal();

                for (int i = 0; i < tempRecipeBook.ids.Count; i++)
                {
                    int index = itemDatabase.GetIndexByID(tempRecipeBook.ids[i]);
                    GUILayout.BeginHorizontal();
                    index = EditorGUILayout.Popup(index, itemsNames.ToArray());
                    tempRecipeBook.ids[i] = itemDatabase.GetIDByIndex(index);
                    if(GUILayout.Button("X"))
                    {
                        tempRecipeBook.ids.RemoveAt(i);
                    }
                    GUILayout.EndHorizontal();
                }
                if(GUILayout.Button("Add"))
                {
                    tempRecipeBook.ids.Add(0);
                }
                GUILayout.EndVertical();
            }


            GUILayout.EndVertical();
            GUILayout.Space(50);

            GUILayout.EndVertical();
        }

        protected override void CallViewList()
        {
            base.CallViewList();
            RecipeBook recipe;
            for (int i = 0; i < database.Count; i++)
            {
                recipe = database.GetByIndex(i);
                GUILayout.BeginHorizontal("Box", GUILayout.Width(225));
                if (!editing)
                {
                    if (GUILayout.Button("Edit", GUILayout.Width(40)))
                    {
                        tempRecipeBook = recipe;
                        index = i;
                        showDetails = true;
                        editing = true;
                    }
                }
                else
                {
                    if (GUILayout.Button("Edit", GUILayout.Width(40)))
                    {
                        EditorUtility.DisplayDialog("Editing", "Finish Editing Current Selection.", "Ok");
                    }
                }

                GUILayout.Label(recipe.name, GUILayout.Width(130));
                GUILayout.EndHorizontal();
            }
        }

        protected override void DisplayButtons()
        {
            base.DisplayButtons();
            if (!showDetails)
            {
                if (GUILayout.Button("Create New"))
                {
                    tempRecipeBook = new RecipeBook();
                    tempRecipeBook.id = database.currentId;
                    showDetails = true;
                }
            }
            else
            {
                if (GUILayout.Button("Save and Return"))
                {
                    if (editing)
                    {
                        showDetails = false;
                        database.Swap(tempRecipeBook, index);
                        tempRecipeBook = null;
                        editing = false;
                    }
                    else
                    {
                        showDetails = false;
                        database.Add(tempRecipeBook);
                        tempRecipeBook = null;
                        editing = false;
                    }
                }
                if (GUILayout.Button("Save and Create Another"))
                {
                    if (editing)
                    {
                        showDetails = true;
                        database.Swap(tempRecipeBook, index);
                        editing = false;
                        tempRecipeBook = new RecipeBook();
                        tempRecipeBook.id = database.currentId;
                    }
                    else
                    {
                        showDetails = true;
                        database.Add(tempRecipeBook);
                        tempRecipeBook = new RecipeBook();
                        tempRecipeBook.id = database.currentId;
                    }
                }
                if (editing)
                {
                    if (GUILayout.Button("Delete"))
                    {
                        if (EditorUtility.DisplayDialog("Confirm Delete", "Are you sure you want to delete this?", "Yes", "Cancel"))
                        {
                            database.Delete(index);
                            showDetails = false;
                            editing = false;
                            tempRecipeBook = null;
                        }
                    }
                }
                if (GUILayout.Button("Cancel"))
                {
                    showDetails = false;
                    editing = false;
                    tempRecipeBook = null;
                }
            }
        }

        protected override void BottomBar()
        {
            base.BottomBar();
        }
    }
}
