using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/// <summary>
/// Class for the Plates
/// </summary>
public class Plate : Interactable
{
    Vegetables heldVegetable = Vegetables.None; //which vegetable is held by the plate
    public SpriteRenderer sprite;   //sprite renderer for the vegetable

    // Update is called once per frame
    void Update()
    {
        if(player != null && Input.GetKeyDown(player.pm.interact))  //if player isn't null & the interact button is pressed
        {
            if(heldVegetable == Vegetables.None && player.vegetables[0].vegetable != Vegetables.None)   //if held vegetable isnt None & the player is holding a vegetable
            {
                heldVegetable = player.vegetables[0].vegetable; //set vegetable to what the player is holding
                player.RemoveVegetable();   //remove vegetable from the player hand
                sprite.sprite = GameManager.instance.GetSprite(heldVegetable);  //set the sprite to the vegetable
            }
            else if(heldVegetable != Vegetables.None && player.HasSlotOpen())   //if the held vegetable isn't null & player has an open hand
            {
                player.AddVegetable(heldVegetable); //add the held vegetable to the players hand
                heldVegetable = Vegetables.None;    //set held vegetable to none
                sprite.sprite = GameManager.instance.GetSprite(heldVegetable);  //set the sprite to none
            }
        }
    }
}
