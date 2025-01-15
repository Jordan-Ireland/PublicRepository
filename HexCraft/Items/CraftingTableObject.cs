using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CraftingTableObject : MonoBehaviour, IInteractable
{
    public int recipeBookId;
    RecipeBook recipes;

    private void Start()
    {
        if (GameManager.recipeDatabase != null)
        {
            recipes = GameManager.recipeDatabase.GetById(recipeBookId);
        }
    }

    public void Use(float dmg, TOOLTYPE type)
    {
        recipes.Use();
    }
}
