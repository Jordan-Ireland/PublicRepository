using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/// <summary>
/// To be used for the chopping boards
/// </summary>
public class ChoppingBoard : Interactable
{
    public Vegetables[] vegetables = new Vegetables[3];//List of the 3 vegetables that can be used
    public bool beingUsed = false;  //Is this currently being used by a player?

    public SpriteRenderer[] renderers = new SpriteRenderer[3];  //List of the 3 sprite renderers for the vegeetables

    // Update is called once per frame
    void Update()
    {
        if (player != null && Input.GetKey(player.pm.interact) && !beingUsed)   //if player is not null & they're pressing the interact button & the chopping board isn't being used by another player
        {
            beingUsed = true;   //set being used to true
            if (player.vegetables[0].vegetable != Vegetables.None) //if the first vegetable the player is holding isn't none
            {
                StartCoroutine(Chop());   //start using the bhopping board
            }
            else if (vegetables[0] != Vegetables.None && vegetables[1] != Vegetables.None) //if the chopping board isn't holding a vegetable
            {
                GetSaladCombo();    //pick up a salad
            }
        }
    }

    /// <summary>
    /// Resets the chopping board back to default
    /// </summary>
    void Reset()
    {
        for (int i = 0; i < vegetables.Length; i++) //loop through the vegetabls
        {
            vegetables[i] = Vegetables.None;    //set them to None
            renderers[i].sprite = null;         //set the sprite renderers to null
        }
    }

    /// <summary>
    /// Checks if the chopping board has a valid salad combo 
    /// </summary>
    void GetSaladCombo()
    {
        foreach (KeyValuePair<int, List<Vegetables>> combo in SaladCombo.combos)    //loop over the main list of combos
        {
            if(ComboContains(combo.Value))  //Check if it is a valid combo
            {
                player.currentSalad = combo.Key;    //set current sald to the index
                player.hasSalad = true;             //set has salad to true
                player.saladRenderer.gameObject.SetActive(true);    //Set the player salad renderer to active
                Reset();    //reset the chopping board
                break;  //exit the loop
            }
        }
    }

    /// <summary>
    /// Checks if the ingredients on the chopping board is a valid recipe
    /// </summary>
    /// <param name="recipe">List of ingredients that make up a recipe</param>
    /// <returns></returns>
    bool ComboContains(List<Vegetables> recipe)
    {
        foreach (Vegetables veg in vegetables)  //loop over the chopping board ingredients
        {
            if (veg != Vegetables.None && !recipe.Contains(veg))    //checks if ingredient doesn't = none & if the recipe doesn't contain the ingredient
            {
                return false;   //returns false
            }
        }
        return true;    //otherwise return true
    }

    /// <summary>
    /// Checks if the player has exited the trigger for the chopping board
    /// </summary>
    /// <param name="collision"></param>
    protected override void OnTriggerExit2D(Collider2D collision)
    {
        if (player != null && collision.GetComponent<Player>() == player)
        {
            beingUsed = false;
            player = null;
        }
    }

    /// <summary>
    /// Coroutine for the chopping action.
    /// </summary>
    /// <returns>null</returns>
    IEnumerator Chop()
    {
        player.pm.canMove = false;  //sets player movement to false
        bool added = AddVegetable(player.vegetables[0].vegetable);  //checks if vegetable in players hand is valid
        if (added)  //if it is valid
        {
            player.RemoveVegetable();   //remove vegetable from players hand
            StartCoroutine(player.UpdateSlider());  //starts coroutine to update the player progress bar
            yield return new WaitForSeconds(1); //wait 1 second
        }
        player.pm.canMove = true;   //set player movement to true
        beingUsed = false;  //set the chopping board to not being used
        yield break;    //break of of coroutine
    }

    /// <summary>
    /// Checks if vegetable on cutting board gets added
    /// </summary>
    /// <param name="_veg">Vegetable to add</param>
    /// <returns>true if vegetable was added, false if vegetable wasn't added</returns>
    bool AddVegetable(Vegetables _veg)
    {
        for (int i = 0; i < vegetables.Length; i++) //loop over the chopping board slots
        {
            if(vegetables[i] == Vegetables.None)    //if found empty slot
            {   
                vegetables[i] = _veg;   //add the vegetable
                renderers[i].sprite = GameManager.instance.GetSprite(_veg); //set the sprite to equal the veetable sprite
                return true;    //return true
            }
        }
        return false;   //return false
    }
}
