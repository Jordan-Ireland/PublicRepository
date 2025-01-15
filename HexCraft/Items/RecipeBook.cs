using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public class RecipeBook
{
    public string name = "Temp Name";
    public int id = -1;
    public List<int> ids = new List<int>();

    public void Use()
    {
        //show crafting menu
        GameManager.instance.recipes = this;
        UIManager.instance.OpenCrafting();
    }
}
