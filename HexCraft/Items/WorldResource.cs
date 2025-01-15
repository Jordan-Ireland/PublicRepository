using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public enum ResourceType { Wood, Rock, Plant }
public class WorldResource : MonoBehaviour, IInteractable
{
    public SO_Resource resourceSO;
    public ResourceType resourceType;
    Material originalMat;

    protected float timeSinceDead = 0;
    int amount;
    protected bool dead = false;
    protected bool startedGrowing = false;
    protected float currentHealth;

    public bool IsDead { get { return dead; } }
    public float TimeSinceDead { get { return timeSinceDead; } }

    protected Vector3 initialScale;
    protected Hexagon hex;

    protected MeshRenderer mesh;
    protected Collider col;

    public virtual void Init(bool dead, float timeSinceDead, Hexagon hex, bool isEnemy = false, int enemyID = -1)
    {
        this.hex = hex;
        //Debug.Log(hex.name);
        //transform.parent = hex.transform;
        this.dead = dead;
        Setup();
        if(this.dead)
        {
            Die(timeSinceDead);
        }
    }

    // Start is called before the first frame update
    void Start()
    {
        Setup();
        originalMat = GetComponent<MeshRenderer>().materials[0];
    }

    public void Use(float dmg, TOOLTYPE type)
    {
        DoDmg(dmg, type);
    }

    protected virtual void Setup()
    {
        mesh = GetComponent<MeshRenderer>();
        col = GetComponent<Collider>();
        initialScale = Vector3.one;
        amount = Random.Range((int)resourceSO.amountRange.x, (int)resourceSO.amountRange.y);
        currentHealth = resourceSO.maxHealth;
    }

    private void Update()
    {
        if (dead && !startedGrowing)
        {
            StartCoroutine(Grow());
        }
    }

    protected virtual IEnumerator Grow()
    {
        GetComponent<MeshRenderer>().sharedMaterial = GameManager.instance.growingMaterial;
        startedGrowing = true;
        //float time = 0;
        Vector3 startPosition = transform.localScale;
        while (timeSinceDead < resourceSO.timeToReset)
        {
            transform.localScale = Vector3.Lerp(startPosition, initialScale, timeSinceDead / resourceSO.timeToReset);
            timeSinceDead += Time.deltaTime;
            yield return null;
        }
        transform.localScale = initialScale;
        GetComponent<MeshRenderer>().sharedMaterial = originalMat;
        col.enabled = true;
        dead = false;
        amount = Random.Range((int)resourceSO.amountRange.x, (int)resourceSO.amountRange.y);
        currentHealth = resourceSO.maxHealth;
        startedGrowing = false;
    }

    public virtual void DoDmg(float dmgAmount, TOOLTYPE toolType)
    {
        //Debug.Log(dmgAmount);
        if (dmgAmount == 0) dmgAmount = 1;
        dmgAmount *= (toolType == resourceSO.itemProperty.ToolType) ? resourceSO.itemProperty.ToolSpeed : resourceSO.itemProperty.NonToolSpeed;
        currentHealth -= dmgAmount;
        //int a = (int)(amount * ((resourceSO.maxHealth - currentHealth) / resourceSO.maxHealth));
        //if(a > 0)
        //    GameManager.playerInstance.inventory.AddItemToInventory(resourceSO.itemID, a);
        // amount -= a;
        //Debug.Log(a + " / " + amount);
        if (currentHealth <= 0)
        {
            //die
            col.enabled = false;
            Die();
        }
        GameObject go = Instantiate(GameManager.instance.miningParticleEffects[0], transform);
        go.transform.position = GameManager.playerInstance.transform.position + (GameManager.playerInstance.transform.forward * 2);

        switch (resourceType)
        {
            case ResourceType.Wood:
                break;
            case ResourceType.Rock:
                break;
            case ResourceType.Plant:
                break;
        }
    }

    public virtual void Die()
    {
        if (!resourceSO.instant)
        {
            switch (resourceType)
            {
                case ResourceType.Wood:
                    StartCoroutine(FallDown());
                    break;
                case ResourceType.Rock:
                case ResourceType.Plant:
                    col.enabled = false;
                    dead = true;
                    timeSinceDead = 0;
                    //mesh.enabled = false;
                    transform.localScale = Vector3.zero;
                    break;
            }
        }else
        {
            col.enabled = false;
            dead = true;
            timeSinceDead = 0;
            transform.localScale = Vector3.zero;
        }
        GameManager.playerInstance.inventory.AddItemToInventory(resourceSO.itemToGive, amount);
        GameManager.playerInstance.AddExp(resourceSO.expToGive);
    }
    public virtual void Die(float timeSinceDead)
    {
        transform.localScale = Vector3.one * (timeSinceDead / resourceSO.timeToReset);
        col.enabled = false;
        startedGrowing = false;
        dead = true;
        this.timeSinceDead = timeSinceDead;
        //mesh.enabled = false;
        //transform.localScale = Vector3.zero;
    }

    IEnumerator FallDown()
    {
        Vector3 position = transform.position;
        Quaternion startRotation = transform.rotation;
        float rotZ = 0;
        float rotX = 0;
        float rotXScale = Random.Range(-1f, 1f), rotZScale = Random.Range(-1f, 1f);
        float timer = 0;
        while (timer < 1.5f)
        {
            Vector3 rotation = new Vector3(rotX, transform.rotation.eulerAngles.y, rotZ);
            transform.rotation = Quaternion.Euler(rotation);
            rotZ += rotZScale * 50 * Time.deltaTime;
            rotX += rotXScale * 50 * Time.deltaTime;
            //transform.position += Vector3.up * Time.deltaTime;
            timer += Time.deltaTime;
            yield return new WaitForEndOfFrame();
        }
        transform.position = position;
        transform.rotation = startRotation;
        col.enabled = false;
        dead = true;
        timeSinceDead = 0;
        transform.localScale = Vector3.zero;
        yield break;
    }
}
