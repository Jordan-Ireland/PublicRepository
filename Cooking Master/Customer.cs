using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class Customer : Interactable
{
    public SpriteRenderer[] sprites = new SpriteRenderer[3];
    public Slider timeSlider;
    public int saladWanted = -1;
    public bool isAngry = false;
    float timeMultiplier = 1;
    float time = 60;
    float maxTime = 60;
    public List<Player> angryAtPlayers = new List<Player>();

    private void Start()
    {
        GameManager.customers.Add(this);
        time = maxTime;
        PickSalad();
    }

    // Update is called once per frame
    void Update()
    {
        if(player != null && player.hasSalad && Input.GetKeyDown(player.pm.interact))
        {
            DeliverSalad();
            //add points
        }
        if(time < 0)
        {
            time = 0;
            StartCoroutine(Leave());
        }else if(time > 0)
        {
            time -= Time.deltaTime * timeMultiplier;
            timeSlider.value = time / maxTime;
        } 

    }

    IEnumerator Leave()
    {
        GameManager.AddScore(true, -5);
        GameManager.AddScore(false, -5);
        while (transform.position.y <= 7)
        {
            transform.position = Vector3.Lerp(transform.position, new Vector3(transform.position.x, 10, transform.position.z), Time.deltaTime * 5);
            yield return new WaitForEndOfFrame();
        }
        gameObject.SetActive(false);
    }

    void PickSalad()
    {
        for (int x = 0; x < sprites.Length; x++)
        {
            sprites[x].sprite = null;
        }
        saladWanted = Random.Range(0, SaladCombo.combos.Keys.Count);
        int i = 0;
        foreach (Vegetables ingredient in SaladCombo.combos[saladWanted])
        {
            sprites[i].sprite = GameManager.instance.GetSprite(ingredient);
            i++;
        }
    }

    void DeliverSalad()
    {
        if(player.currentSalad == saladWanted)
        {
            //plus points
            if(timeSlider.value >= 0.7f)
            {
                GameManager.instance.DropBuff(player);
            }
            GameManager.AddScore(player.isPlayer1, 5);
            angryAtPlayers.Remove(player);
            isAngry = false;
            timeMultiplier = 1;
        }
        else
        {
            GameManager.AddScore(player.isPlayer1, -5);
            angryAtPlayers.Add(player);
            isAngry = true;
            timeMultiplier = 1.5f;
        }
        time = maxTime;
        player.RemoveSalad();
        PickSalad();
    }
}
