using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public enum Vegetables { None, Lettuce, Tomato, Onion } //enum for all the vegetables
/// <summary>
/// Class to hold player variables
/// </summary>
public class Player : MonoBehaviour
{
    public VegetableWorldObj[] vegetables = new VegetableWorldObj[2];
    public PlayerMovement pm;
    public bool hasSalad = false;
    public SpriteRenderer saladRenderer;
    public bool isPlayer1 = true;
    public int currentSalad = -1;
    public Slider choppingSlider;

    // Start is called before the first frame update
    void Start()
    {
        pm = GetComponent<PlayerMovement>();
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    /// <summary>
    /// remove salad from the player hand
    /// </summary>
    public void RemoveSalad()
    {
        saladRenderer.gameObject.SetActive(false);
        currentSalad = -1;
        hasSalad = false;
    }

    /// <summary>
    /// removes the vegetable from the player hand
    /// </summary>
    public void RemoveVegetable()
    {
        vegetables[0].RemoveVegetable();
        vegetables[0].AddVegetable(vegetables[1].vegetable);
        vegetables[1].RemoveVegetable();
    }

    /// <summary>
    /// Adds a vegetable to the player hand
    /// </summary>
    /// <param name="veg">Vegetable that is used</param>
    public void AddVegetable(Vegetables veg)
    {
        if(vegetables[0].vegetable == Vegetables.None)
        {
            vegetables[0].AddVegetable(veg);
        }else if(vegetables[1].vegetable == Vegetables.None)
        {
            vegetables[1].AddVegetable(veg);
        }
    }

    /// <summary>
    /// checks if player has an open hand
    /// </summary>
    /// <returns>true if slot is open, false if no slot available</returns>
    public bool HasSlotOpen()
    {
        for (int i = 0; i < vegetables.Length; i++)
        {
            if(vegetables[i].vegetable == Vegetables.None)
            {
                return true;
            }
        }
        return false;
    }

    /// <summary>
    /// Coroutine to update slider UI for the chopping board
    /// </summary>
    /// <returns></returns>
    public IEnumerator UpdateSlider()
    {
        choppingSlider.gameObject.SetActive(true);
        choppingSlider.value = 0;
        while (choppingSlider.value < 1)
        {
            choppingSlider.value += Time.deltaTime;
            yield return new WaitForEndOfFrame();
        }
        choppingSlider.gameObject.SetActive(false);
        yield break;
    }
}

/// <summary>
/// Struct to hold the renderer and the vegetable
/// </summary>
[System.Serializable]
public struct VegetableWorldObj
{
    public SpriteRenderer spriteRend;
    public Vegetables vegetable;

    public void AddVegetable(Vegetables _veg)
    {
        Sprite _sprite = GameManager.instance.GetSprite(_veg);
        spriteRend.sprite = _sprite;
        vegetable = _veg;
    }

    public void RemoveVegetable()
    {
        spriteRend.sprite = null;
        vegetable = Vegetables.None;
    }
}
